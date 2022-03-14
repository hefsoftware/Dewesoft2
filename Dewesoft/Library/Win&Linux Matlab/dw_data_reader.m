% Matlab Dewesoft import. DWDataReader
%
% v1.0
% supports basic parameters transfer (sample rate,...)
% supports direct buffer data (synch and asynch)
% channel names, colors, timestamps
%
% v1.1
% added event list support
%
% v1.2
% 64 bit support 
%
% v1.3 
% added reading of IB buffers (min, max, rms, ave)
%
% v1.4
% index for channels for reading reduced data was wrong in version 1.3
%
% v1.5 added reading of header data from file
%
% v1.6
% Rewritten for better MATLAB style, save memory by better use of libpointer, overall code clarity, and documentation.
%
%
%% Using the DWDataReader functions generally:
% The typical scenario for using DWDataReader library functions:
% 1.    Load the dll using loadlibrary.
% 1.	Call DWInit to initialize the dynamic link library.
% 2.	Call DWOpenDataFile to open DeweSoft data file (*.d7d, *d7z, *.dxd, *.dxz).
% 3.	Call DWGetChannelListCount to get number of channels.
% 4.	Allocate the DWChannel like structures then loop calling DWGetChannelListItem to get channel list properties and 
%           data.
% 5.	Call DWGetScaledSamplesCount to get number of all samples.
% 6.	Allocate memory (data and time_stamp) then call DWGetScaledSamples.
% 7.	Call DWCloseDataFile to close data file.
% 8.	Call DWDeInit to clear and close dynamic link library.
%
%% Notes
% 1.    libpointer creates lib.pointer objects. The Dewesoft DWDataReader requires pointers have been allocated before
%       passing them to DWDataReader functions. To check if a pointer is uninitialized in MATLAB, use the ".isNull"
%       property. For instance:
%
%       a = libpointer('int32Ptr') 
%       a.isNull
%       % or
%       b = lib.pointer('int32Ptr')
%       b.isNull
%       
%       To know what kind of pointer to use, check the function signature for the function you want use. See the "Output
%       DLL function Lists" section below.
%
% 2.    Once a library is loaded with loadlibrary(), typing "lib." and pressing tab to show all the possible struct types
%       defined by the header via tab completion. This isn't documented. For instance, once DWDataReaderLib64.dll is
%       loaded, the following is available as "lib." tab completion:
%       
%       DWArrayInfo
%       DWChannel
%       DWDataReaderLib64
%       DWEvent
%       DWFileInfo
%       DWReducedValue
%       pointer
%
%       DWArrayInfo, DWChannel, DWEvent, DWFileInfo, and DWReducedValue are all struct types defined in
%       DWDataReaderLib.h. DWDataReaderLib64 shows up because loadlibrary('DWDataReaderLib64',___) was called. pointer
%       is the builtin lib.pointer type.
%
%       Creating a lib.DWDataReaderLib64 object allows access to the list of callable functions. For instance, the code
%       below displays all the methods of lib.DWDataReaderLib64. Also, "a." allows tab completion of function names.
%       The lib.DWDataReaderLib64 object will need cleared before unloading the library.
%
%       a = lib.DWDataReaderLib64;
%       methods(a)
%
%       Using a.functioname works instead of using calllib('libraryname', 'functionName', ___). Instead, this replaces a
%       calllib call:
%
%       a.function(___)
%
% 3.    DWStatus is an enum in DWDataReaderLib.h. Since DWStatus is always an output, it is always a character array in
%       MATLAB. MATLAB converts the enum to its char equivalent.
%
% 4.    MATLAB does not copy an array if the data is not modified. i.e. If array1 is a 1GB array, array2 = array1 does
%       not copy the data until the data in array2 or array1 is modified. The reason this is important is because when
%       we are using libpointer below and then assign the value to a field in a structured array, MATLAB underneath in
%       RAM is not copying the array. If this isn't making sense then uncomment the example below and test it out. We
%       make a 1GB array, array1, and we copy it 100 times into cellArray. This should require 100GB of memory and crash
%       MATLAB on most computers. However, check the memory that MATLAB is using and it won't be 100GB more memory. It
%       will be 1GB more of memory.
% 
%       oneGigaByte = 1024^3;
%       DOUBLE_BYTES = 8;
%       n = ceil(oneGigaByte/DOUBLE_BYTES);
%       array1 = rand(n,1);
%       cellArray = cell(100,1);
%       for i = 1:100
%           cellArray{i} = array1;
%       end
%       variableInfo = whos('cellArray');
%       fprintf('MATLAB claims cellArray is taking up %g[GB]. Is it realy?\n', variableInfo.bytes/oneGigaByte);
%       fprintf('Use the memory() function in MATLAB on Windows to see how\n');
%       fprintf('much memory is actually used. Or on Linux or Mac, use on of\n');
%       fprintf('the OS tools to check MATLAB''s memory Usage.\n');
%
%       Understanding how MATLAB handles memory is important so large amounts of data are not copied in MATLAB when
%       working with large Dewesoft files.
%
% 5.    Working with cstring pointers is a little tricky. If you pass a cstring pointer to calllib in MATLAB, the
%       cstring pointer value is never updated in MATLAB. To get the modified cstring, you must get them on the left
%       hand side of a calllib call. For example:
%
%       n = 200;
%       a = libpointer('cstring',blanks(n));
%       calllib('myLibrary','myFunction', a, n); % this does not work. a.Value is not modified.
%       b = calllib('myLibrary','myFunction', a, n); % this works. a is still unmodified. b is the new string.
%
%       Therefore, just pass in a char on the right and get back char on the left like this:
% 
%       b = calllib('myLibrary','myFunction', blanks(n), n);
%
% 6.    calllib does not handle passing in or out structured arrays except for simple types; char is not considered a
%       simple type. In the code below, we call functions in the DLL, loop ,and populating a structured array in
%       MATLAB. 
%
%       There is a possibility that populating a struct can be done a lot faster by creating a C mexFunction and using
%       the C MEX API to create MATLAB struct arrays and then pass the struct array out of C into MATLAB. However, the
%       code here does work as is and is reasonably fast.

%% setup
% clear all variables
clear; 
% clear command window.
clc;
% close all figures
close all;

%% Helpful anonymous functions
% This little function gets the name of its inputs.
getname = @(x) inputname(1);

%% Load dll
% if 64-bit then load 64-bit dll 
if (contains(computer, '64') == 1) 
    DWDataLibFile = 'DWDataReaderLib64'; 
else
    DWDataLibFile = 'DWDataReaderLib'; 
end

% check if the dll/so is already loaded
if ~libisloaded(DWDataLibFile)
    % load dll with header file
    loadlibrary(DWDataLibFile,'DWDataReaderLib.h');
else
    % The DLL has already been loaded.
end

% The following line of code allows tab complete of "DWDataLib." later. Use "lib." and tab to tab complete struct types.
DWDataLib = lib.(DWDataLibFile);

%% Output DLL function Lists
% list all dll functions in command window
libfunctions(DWDataLib,'-full');

% Display shared C library function signatures in window. same information as libfunctions but displayed cleaner.
libfunctionsview(DWDataLib);

%% Initiate DWDataReader
% This function call must be made prior to making any other calls.
DWStatus = DWDataLib.DWInit();
if strcmpi(DWStatus, 'DWSTAT_ERROR')
    % DWInit has already been called previously. Don't worry about this.
    % The most common case is that a MATLAB code error occurred after a DWInit was called leaving the DLL initialized.
    % If there is a different problem, then the DLL will throw errors on subsequent calls maybe when opening a file.
    % 
    % An alternative approach is to cleanup and start over. Call DWCloseDataFile, DWDeInit, and then DWInit. Uncomment
    % the code block below to use the alternative.
    % 
    % DWDataLib.DWCloseDataFile();
    % DWDataLib.DWDeInit();
    % DWStatus = DWDataLib.DWInit();
    % statusCheck(DWStatus);
else
    statusCheck(DWStatus);
end

%% Output DLL version
% get version of DWDataReader dll
version = DWDataLib.DWGetVersion();
fprintf('Dll version: %d\n\n', version);

%% Open Dewesoft File
tic; % start timing
% file to read in.
FILE = 'Test.dxd';
fprintf("Opening Dewesoft File: %s\n", FILE);

% Note, that we are using a trick when we call DWOpenDataFile. When a C function takes a struct pointer, MATLAB can take
% a structure input to calllib and then it will setup the structure correctly and output it. Therefore, we pass in an
% empty structure to calllib. This avoids having to setup a pointer without extra memory cost.
[DWStatus,~,info_data] = DWDataLib.DWOpenDataFile(FILE, struct());
if strcmpi(DWStatus,'DWSTAT_ERROR_FILE_ALREADY_IN_USE')
    % This means a file was already open. It may be the one we want or it may not. The best way to recover is to close
    % the current file and then open the file we want. In more productive code, you may want to track the currently open
    % file in MATLAB so you don't have to close the file if the same file is open.
    DWDataLib.DWCloseDataFile();
    [DWStatus,~,info_data] = DWDataLib.DWOpenDataFile(FILE, struct());
else
    % do nothing.
end
statusCheck(DWStatus);

fprintf('\nDWFileInfo:\n');
fprintf('Sample rate: %d[Hz]\n', info_data.sample_rate);
fprintf('Start store time: %s\n', datetime(datetime(info_data.start_store_time,'ConvertFrom','excel','TimeZone','UTC'),'TimeZone','local'));
fprintf('File duration: %s\n\n', seconds(info_data.duration));

%% Read in Events
% go through all events in file
event_count = DWDataLib.DWGetEventListCount();

if event_count > 0
    % calling libstruct, then converting to MATLAB structure setups up same fields the same as DWEvent. Assigning the
    % right hand side to all the element of a MATLAB structured array on the left hand sided setups the MATLAB structure
    % correctly. i.e. It's MATLAB's way of setting up a structured array in one line of clean code.
    event_list(1:event_count) = struct(lib.DWEvent);
    
    % We need to setup pointers to call DWGetEventListItem to avoid extra memory cost. If we look at DWDataReader.h and
    % the definition of DWEvent, we can see the character limit of the event_text. We use the MATLAB function signature for
    % DWGetEventListItem to figure out what type of pointer is needed.
    event_type = libpointer('int32Ptr',0);
    time_stamp = libpointer('doublePtr', 0);
    MAX_EVENT_TEXT_CHARACTERS = 200; % DWEvent definition in DWDataReader.h. Note that, MATLAB auto converts this to the right type when calling calllib.
    empty_string = blanks(MAX_EVENT_TEXT_CHARACTERS);
    for iEventIndex = 1:event_count
        % get the event information and check the status of the call
        % strings are a bit different. Passing libpointer to a character array does not get updated. Therefore, we just use
        % the left hand side to pass out the character arrays
        [DWStatus,~,~,event_text] = DWDataLib.DWGetEventListItem(iEventIndex-1, event_type, time_stamp, empty_string, MAX_EVENT_TEXT_CHARACTERS);
        statusCheck(DWStatus);
        
        % save the information into the event_list
        event_list(iEventIndex).event_type = event_type.Value;
        event_list(iEventIndex).time_stamp = time_stamp.Value;
        event_list(iEventIndex).event_text = event_text;
    end
    
    % convert the event list to a table for ease of viewing.
    event_list = struct2table(event_list,'AsArray',true);
    fprintf('Event List:\n')
    disp(event_list)
    
    % ALWAYS clean up your libpointers!!! This is similar to calling free() in C.
    clear(getname(event_type), getname(time_stamp));
    
end


%% Channel List Count
% get the channel count
channel_count = DWDataLib.DWGetChannelListCount();
if channel_count==-1 % check for error
    error('DWGetChannelListCount returns -1 if there was an error.');
end
fprintf('Number of channels: %d\n', channel_count);

%% Channel Information, Full Speed Data, and Reduced Data

if channel_count > 0
    % calling libstruct, then converting to MATLAB structure setups the same fields as DWChannel. Assigning the right
    % hand side to all the element of a MATLAB structured array on the left hand sided of the assignment setups the
    % MATLAB structure correctly and populates the fields with the same data. i.e. It's MATLAB's way of setting up a
    % structured array in one line of clean code.
    channel_list(1:channel_count) = struct(lib.DWChannel);
    
    % add fields for the time and data of each channel.
    channel_list(channel_count).time = [];
    channel_list(channel_count).data = [];
    channel_list(channel_count).sample_count = [];
    channel_list(channel_count).time_reduced = [];
    channel_list(channel_count).data_reduced = [];
    channel_list(channel_count).block_size_reduced = [];
    channel_list(channel_count).sample_count_reduced = [];
    
    % remove the data_type field because it will not be populated or used
    channel_list = rmfield(channel_list,'data_type');
    
    % We need to setup pointers to call DWGetChannelListItem to avoid extra memory cost. If we look at DWDataReader.h
    % and the definition of DWChannel, we can see the character limits of the name, unit, and description. We use the
    % MATLAB function signature for DWGetChannelListItem to figure out what type of pointers are needed.
    MAX_CHAR_SIZE = 200; % Take the max of the chars in the DWChannel definition (name = 100, unit = 20, description = 200).
    index = libpointer('int32Ptr',0);
    empty_string = blanks(MAX_CHAR_SIZE); % use this to tell MATLAB to setup character arrays of the right size and type when calling into calllib
    color = libpointer('int32Ptr', 0);
    array_size = libpointer('int32Ptr', 0);
    for iChannel = 1:channel_count
        % strings are a bit different. Passing libpointer to a character array does not get updated. Therefore, we just
        % use the left hand side to pass out the character arrays
        [DWStatus,~,name,unit,description] = DWDataLib.DWGetChannelListItem(iChannel-1, index, empty_string, empty_string, empty_string, color, array_size, MAX_CHAR_SIZE);
        statusCheck(DWStatus);
        
        % get number of samples for the current channel
        sample_count = DWDataLib.DWGetScaledSamplesCount(index.Value);
        
        % save the channel information
        channel_list(iChannel).index = index.Value;
        channel_list(iChannel).name = name;
        channel_list(iChannel).unit = unit;
        channel_list(iChannel).description = description;
        channel_list(iChannel).color = color.Value;
        channel_list(iChannel).array_size = array_size.Value;
        channel_list(iChannel).sample_count = sample_count;
        
        if sample_count > 0
            % if there is some samples in the data file then read sample values and corresponding time stamps.
            data = libpointer('doublePtr', zeros(sample_count*int64(array_size.Value),1));
            time = libpointer('doublePtr', zeros(sample_count,1));
            offset_position = 0; % use this if you wanted to read in from a certain position in the data array.
            DWStatus = DWDataLib.DWGetScaledSamples(index.Value, offset_position, sample_count, data, time);
            statusCheck(DWStatus);
            
            % save the data and time
            if array_size.Value > 1
                % reshape if its an array
                channel_list(iChannel).data = reshape(data.Value, array_size.Value, sample_count)';
            else
                channel_list(iChannel).data = data.Value;
            end
            channel_list(iChannel).time = time.Value;
        end
        
        
        % read values from reduced buffer
        sample_count_reduced = libpointer('int32Ptr',0);
        block_size_reduced = libpointer('doublePtr',0);
        DWStatus = DWDataLib.DWGetReducedValuesCount(index.Value, sample_count_reduced, block_size_reduced);
        statusCheck(DWStatus);
        
        % save the reduced buffer information
        channel_list(iChannel).block_size_reduced = block_size_reduced.Value;
        channel_list(iChannel).sample_count_reduced = sample_count_reduced.Value;
        
        if sample_count_reduced.Value > 0
            % setup pointers for getting the reduced data
            data_reduced = libpointer('doublePtr', zeros(sample_count_reduced.Value*array_size.Value, 1));
            time_reduced = libpointer('doublePtr', zeros(sample_count_reduced.Value,1));
            offset_position_reduced = 0; % use this if you want to read in from a certain position in the reduced array.
            % Note that you can also use the DWGetReducedMaxValues, DWGetReducedMinValues, and DWGetReducedRMSValues
            % functions to Max, Min, and RMS values for each block
            DWStatus = DWDataLib.DWGetReducedAveValues(index.Value, offset_position_reduced, sample_count_reduced.Value, data_reduced, time_reduced);
            statusCheck(DWStatus);
            
            % save the reduced data and time
            if array_size.Value > 1
                % reshape if its an array
                channel_list(iChannel).data_reduced = reshape(data_reduced.Value, sample_count_reduced.Value, array_size.Value);
            else
                channel_list(iChannel).data_reduced = data_reduced.Value;
            end
            channel_list(iChannel).time_reduced = time_reduced.Value;
        end
    end
    
    % ALWAYS clean up your lib.pointer's!!! This is similar to calling free() in C.
    clear(getname(index), getname(name), getname(unit), getname(description), getname(color),getname(array_size));
    if exist('data','var')
        clear(getname(data), getname(time));
    end
    if exist('sample_count_reduced','var')
        clear(getname(sample_count_reduced), getname(block_size_reduced));
    end
    if exist('data_reduced','var')
        clear(getname(data_reduced), getname(time_reduced));
    end
    
    % convert channel_list struct to table for easier viewing.
    channel_list = struct2table(channel_list,'AsArray',true);
    fprintf('Channel List:\n')
    disp(channel_list)
    
    % calculate MATLAB color. Note since these are int32's, integer division is in play.
    blue = rem(channel_list.color, int32(256));
    green = rem((channel_list.color - blue) / int32(256), int32(256));
    red = (channel_list.color-blue)/int32(256)^2 - green/int32(256);
    channel_list.rgb_color = uint8([red green blue]); % this can be helpful to compare against Dewesoft
    channel_list.matlab_color = double([red green blue])./255; % What MATLAB needs
    
    % plot 9 channels max. Use the reduced rate
    nGraphs = min(9, double(channel_count));
    GRAPHS_PER_FIGURE = 3;
    nFigures = ceil(nGraphs/GRAPHS_PER_FIGURE);
    for iFigure = 1:nFigures
        figure;
        for iGraph = 1:min((nGraphs - (iFigure-1)*GRAPHS_PER_FIGURE), GRAPHS_PER_FIGURE)
            subplot(3,1,iGraph)
            index = iGraph + GRAPHS_PER_FIGURE*(iFigure-1);
            x = channel_list.time_reduced{index};
            y = channel_list.data_reduced{index};
            plot(x,y,'DisplayName', channel_list.name{index});
            xlabel('Time [sec]');
            ylabel(sprintf('%s [%s]', channel_list.name{index}, channel_list.unit{index}),'Interpreter','none');
            title(sprintf('%s', channel_list.name{index}),'Interpreter','none');
        end
    end
end

%% read header information from file
header_count = DWDataLib.DWGetHeaderEntryCount();

if header_count > 0
    header_list(1:header_count) = struct('name','','unit','','description','','header_info','','index',[],'color',[],'array_size',[]);
    
    MAX_CHAR_SIZE = 200; % Take the max of the chars in the DWChannel definition (name = 100, unit = 20, description = 200).
    index = libpointer('int32Ptr',0);
    empty_string = blanks(MAX_CHAR_SIZE); % use this to tell MATLAB to setup character arrays of the right size and type when calling into calllib
    color = libpointer('int32Ptr', 0);
    array_size = libpointer('int32Ptr', 0);
    MAX_HEADER_CHAR_SIZE = 10000;
    empty_string2 = blanks(MAX_HEADER_CHAR_SIZE);  % use this to tell MATLAB to setup character arrays of the right size and type when calling into calllib
    for iHeaderIndex = 1:header_count
        % strings are a bit different. Passing libpointer to a character array does not get updated. Therefore, we just
        % use the left hand side to pass out the character arrays
        [DWStatus, ~, name, unit, description] = DWDataLib.DWGetHeaderEntryListItem( iHeaderIndex-1, index, empty_string, empty_string, empty_string, color, array_size, MAX_CHAR_SIZE);
        satusCheck(DWStatus);
        
        % strings are a bit different. Passing libpointer to a character array does not get updated. Therefore, we just
        % use the left hand side to pass out the character arrays
        [DWStatus, header_info] = DWDataLib.DWGetHeaderEntryText(index.Value, empty_string2, MAX_HEADER_CHAR_SIZE);
        
        fprintf('Header entry name: %s - Header entry value: %s\n', name, header_info);
        
        % save header information
        header_list(iHeaderIndex).name = name;
        header_list(iHeaderIndex).unit = unit;
        header_list(iHeaderIndex).description = description;
        header_list(iHeaderIndex).header_info = header_info;
        header_list(iHeaderIndex).index = index.Value;
        header_list(iHeaderIndex).color = color.Value;
        header_list(iHeaderIndex).array_size = array_size.Value;
    end
    
    % ALWAYS clean up your libpointers!!! This is similar to calling free() in C.
    clear(getname(index), getname(color), getname(array_size)); 
end

toc; % stop timing

%% Close down the library
fprintf("Closing Dewesoft File: %s\n", FILE);

% close file and deinit reader
DWDataLib.DWCloseDataFile();
DWDataLib.DWDeInit();

% Any "lib." objects must be cleared before unloading the library. Thus, DWDataLib needs cleared.
clear(getname(DWDataLib));

% unload DLL from memory.
% Note, all libpointers and libstructs must be cleared first. This is like calling free() in C.
unloadlibrary(DWDataLibFile);


%%
% If statusCheck() function is not working, you are using an older version of MATLAB. Copy it to a new file and save it
% in the same directory as this script. Delete it below. This should fix the error.

function statusCheck(DWStatus)
% Check the state of DWStatus.
%
%   statusCheck(DWStatus)
%
%% Inputs
% DWStatus - Character array corresponding the enum in DWDataReaderLib.h.
%
%% Description
% DWStatus is an enum in DWDataReaderLib.h. Since DWStatus is always an output, it is always a character array. MATLAB
% converts the enum to its string equivalent.

    switch DWStatus
        case {'DWSTAT_OK', 0}
            % STAT OK
            % Do nothing
        case {'DWSTAT_ERROR', 1}
            error(['DWSTAT_ERROR',': error occurred in the dll']);
        case {'DWSTAT_ERROR_FILE_CANNOT_OPEN', 2}
            error(['DWSTAT_ERROR_FILE_CANNOT_OPEN',': unable to open file']);
        case {'DWSTAT_ERROR_FILE_ALREADY_IN_USE', 3}
            error(['DWSTAT_ERROR_FILE_ALREADY_IN_USE',': file already in use']);
        case {'DWSTAT_ERROR_FILE_CORRUPT', 4}
            error(['DWSTAT_ERROR_FILE_CORRUPT',': file corrupted']);
        case {'DWSTAT_ERROR_NO_MEMORY_ALLOC', 5}
            error(['DWSTAT_ERROR_NO_MEMORY_ALLOC',': memory not allocated']);
        case {'DWSTAT_ERROR_CREATE_DEST_FILE', 6}
            error(['DWSTAT_ERROR_CREATE_DEST_FILE',': error creating destination file (only for d7d, d7z, dxd, dxz files)']);
        case {'DWSTAT_ERROR_EXTRACTING_FILE', 7}
            error(['DWSTAT_ERROR_EXTRACTING_FILE',': error extracting data (only for d7d, d7z, dxd, dxz files)']);
        case {'DWSTAT_ERROR_CANNOT_OPEN_EXTRACTED_FILE', 8}
            error(['DWSTAT_ERROR_CANNOT_OPEN_EXTRACTED_FILE',': error opening extracted file (only for d7d, d7z, dxd, dxz files)']);
        otherwise
            error('Unknown status occurred.\nDWStatus was \n %s', DWStatus);
    end
end
