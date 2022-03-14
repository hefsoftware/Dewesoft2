#include "singlearea.h"
#include "../Band/band.h"
#include "../Band/plotareaband.h"
#include "../Band/placer.h"
#include "../Area/area.h"
#include "../Area/placer.h"
#include <QDebug>
#include <QTransform>
using namespace Plot;
Plot::Band *SingleArea::bandAtPosition(const QPointF &pt)
{
  Plot::Band *ret=nullptr;
  for(int i=0;i<m_placedH.size();i++)
  {
    if(m_placedH[i]->contains(pt))
    {
      ret=m_placedH[i];
      break;
    }
  }
  for(int i=0;i<m_placedV.size();i++)
  {
    if(m_placedV[i]->contains(pt))
    {
      ret=m_placedV[i];
      break;
    }
  }
  return ret;
}

Plot::Band *SingleArea::band(int id)
{
  Plot::Band *ret=nullptr;
  if(m_bandsH.contains(id))
    ret=m_bandsH[id];
  else if(m_bandsV.contains(id))
    ret=m_bandsV[id];
  return ret;
}

Area *SingleArea::area(int id)
{
  return m_areas.value(id);
}



void SingleArea::zoom(const QPointF &pt, double angle, QSet<Plot::Band *> bands)
{
  for(auto it=bands.begin();it!=bands.end();it++)
    (*it)->zoom((*it)->transform().inverted().map(pt), angle);
}


int SingleArea::insertIn(QMap<int, Plot::Band *> &map, Plot::Band *band, int id)
{
  if(id<=0 || m_bandsH.contains(id) || m_bandsV.contains(id) || m_areas.contains(id) || (band->parent()!=nullptr && band->parent()!=this))
    id=0;
  else
  {
    map.insert(id, band);
    Plot::BandPlacer(band).assign(this, id, map==m_bandsH); //.setWidget(this).setId(id);
    invalidateLayout();
  }
  return id;
}

int SingleArea::insertIn(QMap<int, Plot::Band *> &map, Plot::Band *band)
{
  return insertIn(map, band, nextItemId());
}

int SingleArea::insertIn(QMap<int, Area *> &map, Area *area, int id)
{
  if(id<=0 || m_bandsH.contains(id) || m_bandsV.contains(id) || m_areas.contains(id) || (area->parent()!=nullptr && area->parent()!=this))
    id=0;
  else
  {
    map.insert(id, area);
    Plot::ItemPlacer(area).assign(this, id); //.setWidget(this).setId(id);
    invalidateLayout();
  }
  return id;
}

int SingleArea::insertIn(QMap<int, Area *> &map, Area *area)
{
  return insertIn(map, area, nextItemId());
}

int SingleArea::addArea(Area *area)
{
  return insertIn(m_areas, area, nextItemId());
}

void SingleArea::calculateBands(const QMap<int, Plot::Band *> &bands, double size, QList<Plot::Band *> &output)
{
  output.clear();
  if(!bands.isEmpty())
  {
    // Select top priority bands
    QList<Plot::Band *> bandsPriority;

    for(auto it=bands.begin(); it!=bands.end(); it++)
      if(it.value()->visible())
        bandsPriority.append(it.value());
    std::sort(bandsPriority.begin(), bandsPriority.end(), [](Plot::Band *b1, Plot::Band *b2) { return b1->priority()>b2->priority(); } );
    double acceptedMinSize=0.;
    int acceptedNum=0;
    double curW=0;
    for(int i=0,s=0;i<bandsPriority.size();i++)
    {
      curW+=bandsPriority[i]->minHeight();
      if(acceptedMinSize+curW>size)
        break;
      else if(i==bandsPriority.size()-1 || bandsPriority[i+1]->priority()!=bandsPriority[s]->priority())
      {
         acceptedNum=i+1;
         acceptedMinSize+=curW;
         curW=0.;
         s=i+1;
      }
    }
    bandsPriority=bandsPriority.mid(0, acceptedNum);
    double expandSum=0.;
    auto expandTrim=[](double x) { return fmin(fmax(x,1e-5),1e5); };
    auto expandBand=[expandTrim](Plot::Band *band){ return expandTrim(band->expand()); };
    for(int i=0;i<bandsPriority.size();i++) expandSum+=expandBand(bandsPriority[i]);
    auto maxExpandFactor=[expandBand](Plot::Band *band){ return (band->maxHeight()-band->minHeight())/expandBand(band); };
    std::sort(bandsPriority.begin(), bandsPriority.end(), [maxExpandFactor](Plot::Band *b1, Plot::Band *b2) { return maxExpandFactor(b1)<maxExpandFactor(b2); } );
    double remainSize=size;
    QMap<Plot::Band *, double> heights;
    for(int i=0;i<bandsPriority.size();i++)
    {
      double expandFactor=(remainSize-acceptedMinSize)/expandSum;
      auto const cb=bandsPriority[i];
      double curHeight=cb->minHeight()+expandFactor*expandBand(cb);
//      qDebug()<<i<<"Evaluating"<<cb<<curHeight<<"Remain size"<<remainSize<<"Accepted min size"<<acceptedMinSize<<"expand sum"<<expandSum;
      curHeight=fmin(fmax(curHeight,cb->minHeight()),cb->maxHeight());
//      qDebug()<<"  after cut"<<curHeight;
      heights.insert(cb, curHeight);
      remainSize-=curHeight;
      acceptedMinSize-=cb->minHeight();
      expandSum-=expandBand(cb);
    }
    output=bands.values();
    std::stable_sort(output.begin(), output.end(), [](Plot::Band *b1, Plot::Band *b2) { return b1->placeOrder() < b2->placeOrder(); } );
    double top=0.;
    for(int i=0;i<output.size();i++)
    {
      auto cb=output[i];
      Plot::BandPlacer(cb).setTop(top).setHeight(heights.value(cb, 0.)).setShown(heights.contains(cb));
      top+=cb->height();
    }
  }
}

void SingleArea::calculateCorners(int startV, int startH, bool left, bool top)
{
  int incV=left?-1:1, incH=top?-1:1;
  double posH=0., posV=0.;
  auto setH=[left](Plot::Band *band, double val){ if(left)Plot::BandPlacer(band).setBorderLeft(val); else Plot::BandPlacer(band).setBorderRight(val); return band->height();};
  auto setV=[top](Plot::Band *band, double val){ if(top)Plot::BandPlacer(band).setBorderLeft(val); else Plot::BandPlacer(band).setBorderRight(val); return band->height();};
  startV+=incV; startH+=incH;
  for(;;)
  {
    Plot::Band *hinfo=(startH>=0 && startH<m_placedH.size())?m_placedH[startH]:nullptr;
    Plot::Band *vinfo=(startV>=0 && startV<m_placedV.size())?m_placedV[startV]:nullptr;
//    PlotBand *hband=hinfo?hinfo->band():nullptr, *vband=vinfo?vinfo->band():nullptr;

    if(hinfo && vinfo)
    {
      if(hinfo->sidePriority()>=vinfo->sidePriority())
      {
        posV+=setV(vinfo,posH);
        startV+=incV;
      }
      else
      {
        posH+=setH(hinfo, posV);
        startH+=incH;
      }
    }
    else if(vinfo)
    {
      posV+=setV(vinfo,posH);
      startV+=incV;
    }
    else if(hinfo)
    {
      posH+=setH(hinfo, posV);
      startH+=incH;
    }
    else
      break;
  }
}

void SingleArea::relayout()
{
  auto sz=size();
  auto bandsH=m_bandsH, bandsV=m_bandsV;
//  bandsH.insert(0, m_bandPlotH);
//  bandsV.insert(0, m_bandPlotV);
  // First calculates the "vertical" placing
  calculateBands(bandsH, sz.height(), m_placedH);
  calculateBands(bandsV, sz.width(), m_placedV);
  int plotH=-1,plotV=-1;
  QRectF m_plotArea;
  // Finds the plot area
  for(int i=0;i<m_placedH.size();i++)
    if(dynamic_cast<Plot::PlotBandPlotArea *>(m_placedH[i]))
    {
      plotH=i;
      m_plotArea.setTop(m_placedH[i]->top());
      m_plotArea.setHeight(m_placedH[i]->height());
      break;
    }
  for(int i=0;i<m_placedV.size();i++)
    if(dynamic_cast<Plot::PlotBandPlotArea *>(m_placedV[i]))
    {
      plotV=i;
      m_plotArea.setLeft(m_placedV[i]->top());
      m_plotArea.setWidth(m_placedV[i]->height());
      break;
    }
  Q_ASSERT(plotH>=0 && plotV>=0);
  for(int i=0;i<m_placedH.size();i++)
    Plot::BandPlacer(m_placedH[i]).setLeft(m_plotArea.left()).setWidth(m_plotArea.width());
  for(int i=0;i<m_placedV.size();i++)
    Plot::BandPlacer(m_placedV[i]).setLeft(m_plotArea.top()).setWidth(m_plotArea.height());
  calculateCorners(plotV, plotH, false, false);
  calculateCorners(plotV, plotH, false, true);
  calculateCorners(plotV, plotH, true, false);
  calculateCorners(plotV, plotH, true, true);
  qDebug()<<"Layout finished"<<"Plot area"<<m_plotArea;
  m_placedAreas=m_areas.values();
  std::stable_sort(m_placedAreas.begin(), m_placedAreas.end(), [](const Area *a1, const Area *a2) { return a1->zOrder()<a2->zOrder();});
  for(int i=0;i<m_placedH.size();i++)
    Plot::BandPlacer(m_placedH[i]).layoutChanged();
  for(int i=0;i<m_placedV.size();i++)
    Plot::BandPlacer(m_placedV[i]).layoutChanged();
  for(int i=0;i<m_placedAreas.size();i++)
    Plot::AreaPlacer(m_placedAreas[i]).setRect(m_plotArea).layoutChanged();
}

int SingleArea::nextItemId()
{
  return 1+qMax(m_areas.isEmpty()?0:m_areas.lastKey(),
             qMax(m_bandsH.isEmpty()?0:m_bandsH.lastKey(),
                  m_bandsV.isEmpty()?0:m_bandsV.lastKey()));
}

SingleArea::SingleArea(QObject *parent): Manager(parent)
{
  m_bandPlotH=new Plot::PlotBandPlotArea();
  m_bandPlotV=new Plot::PlotBandPlotArea();

  insertIn(m_bandsH,m_bandPlotH);
  insertIn(m_bandsV,m_bandPlotV);

}

//int PlotWidgetEx::addScaleH(HEF::FunctionPtr scale)
//{
//  return insertIn(m_bandsH, new Plot::BandScaleDefault(scale));
//}

//int PlotWidgetEx::addScaleV(HEF::FunctionPtr scale)
//{
//  return insertIn(m_bandsV, new Plot::BandScaleDefault(scale));
//}

//int PlotWidgetEx::addLabelH(const QString &text, double maxHeight)
//{
//  return insertIn(m_bandsH, new Plot::PlotBandLabel(text, maxHeight));
//}

//int PlotWidgetEx::addLabelV(const QString &text, double maxHeight)
//{
//  return insertIn(m_bandsV, new Plot::PlotBandLabel(text, maxHeight));
//}

int SingleArea::addBandH(Plot::Band *band)
{
  int ret=0;
  if(!dynamic_cast<Plot::PlotBandPlotArea *>(band))
    ret=insertIn(m_bandsH, band);
  return ret;
}

int SingleArea::addBandV(Plot::Band *band)
{
  int ret=0;
  if(!dynamic_cast<Plot::PlotBandPlotArea *>(band))
    ret=insertIn(m_bandsV, band);
  return ret;
}

//QSize PlotWidgetEx::minimumSizeHint() const
//{
//  auto sz=minimumSize();
//  return QSize(ceil(sz.width()), ceil(sz.height()))+QSize(1,1);
//}

QSizeF SingleArea::bestSize()
{
  double minWidth(m_bandPlotV->minHeight()), minHeight(m_bandPlotH->minHeight());
  for(auto it=m_bandsH.begin();it!=m_bandsH.end();it++)
    if((*it)->visible())
      minHeight+=qMin(((*it)->maxHeight()+(*it)->minHeight())*.5,2*(*it)->minHeight());
  for(auto it=m_bandsV.begin();it!=m_bandsV.end();it++)
    if((*it)->visible())
      minWidth+=qMin(((*it)->maxHeight()+(*it)->minHeight())*.5,2*(*it)->minHeight());
  return QSize(minWidth, minHeight);
}


QSizeF SingleArea::minimumSize()
{
  double minWidth(0), minHeight(0);
  for(auto it=m_bandsH.begin();it!=m_bandsH.end();it++)
    if((*it)->visible() && (*it)->priority()==Plot::Band::MaxPriority)
      minHeight+=(*it)->minHeight();
  for(auto it=m_bandsV.begin();it!=m_bandsV.end();it++)
    if((*it)->visible() && (*it)->priority()==Plot::Band::MaxPriority)
      minWidth+=(*it)->minHeight();
  return QSizeF(minWidth, minHeight);
}
