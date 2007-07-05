function varargout = StrainTrack(varargin)
% STRAINTRACK M-file for StrainTrack.fig
%      STRAINTRACK, by itself, creates a new STRAINTRACK or raises the existing
%      singleton*.
%
%      H = STRAINTRACK returns the handle to a new STRAINTRACK or the handle to
%      the existing singleton*.
%
%      STRAINTRACK('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in STRAINTRACK.M with the given input arguments.
%
%      STRAINTRACK('Property','Value',...) creates a new STRAINTRACK or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before StrainTrack_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to StrainTrack_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help StrainTrack

% Last Modified by GUIDE v2.5 03-Jul-2007 18:15:29

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @StrainTrack_OpeningFcn, ...
                   'gui_OutputFcn',  @StrainTrack_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before StrainTrack is made visible.
function StrainTrack_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to StrainTrack (see VARARGIN)

% Set up application variables
handles.images = varargin{1};
handles.tracks = varargin{2};
handles.index = 1;
handles.minIdx = 1;
handles.maxIdx = size(handles.images, 3);

% Set up strain computation variables
handles.refIdx = 1;
handles.movIdx = 2;
handles.startFrame = 1;
handles.endFrame = handles.maxIdx;

% Set up GUI components based on image bounds
set(handles.slideImageIndex, 'Min',   1);
set(handles.slideImageIndex, 'Value', 1);
set(handles.slideImageIndex, 'Max',   handles.maxIdx);
stride = 1 / (handles.maxIdx-1);
set(handles.slideImageIndex, 'SliderStep', [stride, stride]);

% Choose default command line output for StrainTrack
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);
UpdateGUI(handles);

% UIWAIT makes StrainTrack wait for user response (see UIRESUME)
% uiwait(handles.figure1);

function UpdateGUI(handles)
set(handles.slideImageIndex, 'Value', handles.index);
set(handles.textImageIndex, 'String', num2str(handles.index));
set(handles.textStartFrame, 'String', num2str(handles.startFrame));
set(handles.textEndFrame, 'String', num2str(handles.endFrame));
ShowFeatureFrame(handles.images, handles.tracks, handles.index);
hold on;
if (handles.tracks(handles.refIdx, 4, handles.index))
    plot(handles.tracks(handles.refIdx, 2, handles.index), handles.tracks(handles.refIdx, 1, handles.index), 'r*');
    set(handles.textRefFeature, 'String', sprintf('Idx: %4d\tPos: [%3.2f, %3.2f]', handles.refIdx, handles.tracks(handles.refIdx, 1, handles.index), handles.tracks(handles.refIdx, 2, handles.index)));
else
    set(handles.textRefFeature, 'String', 'Feature is not in this frame');
end;
if (handles.tracks(handles.movIdx, 4, handles.index))
    plot(handles.tracks(handles.movIdx, 2, handles.index), handles.tracks(handles.movIdx, 1, handles.index), 'g*');
    set(handles.textMovFeature, 'String', sprintf('Idx: %4d\tPos: [%3.2f, %3.2f]', handles.movIdx, handles.tracks(handles.movIdx, 1, handles.index), handles.tracks(handles.movIdx, 2, handles.index)));
else
    set(handles.textMovFeature, 'String', 'Feature is not in this frame');
end;
hold off;
UpdateStrain(handles);

function UpdateStrain(handles)
[S, Lo, dL, L] = ComputeFeatureStrain(handles.tracks, [handles.refIdx handles.movIdx]', [handles.startFrame handles.endFrame]');
set(handles.textStrain, 'String', sprintf('[%3.4f,\t%3.4f]', S(1), S(2)));
set(handles.textDeltaLength, 'String', sprintf('[%3.4f,\t%3.4f]', dL(1), dL(2)));
set(handles.textLengthStart, 'String', sprintf('[%3.4f,\t%3.4f]', Lo(1), Lo(2)));
set(handles.textLengthEnd, 'String', sprintf('[%3.4f,\t%3.4f]', L(1), L(2)));

% --- Outputs from this function are returned to the command line.
function varargout = StrainTrack_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on slider movement.
function slideImageIndex_Callback(hObject, eventdata, handles)
% hObject    handle to slideImageIndex (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Enforce integer indices
handles.index = round(get(hObject, 'Value'));

% Update GUI data
guidata(hObject, handles);
UpdateGUI(handles);


% --- Executes during object creation, after setting all properties.
function slideImageIndex_CreateFcn(hObject, eventdata, handles)
% hObject    handle to slideImageIndex (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end



function textImageIndex_Callback(hObject, eventdata, handles)
% hObject    handle to textImageIndex (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Enforce valid integer values
idx = round(str2double(get(hObject, 'String')));
idx = max(1,idx);
idx = min(idx, handles.maxIdx);

% Update GUI data
handles.index = idx;
guidata(hObject, handles);
UpdateGUI(handles);


% --- Executes during object creation, after setting all properties.
function textImageIndex_CreateFcn(hObject, eventdata, handles)
% hObject    handle to textImageIndex (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in btnReferenceFeature.
function btnReferenceFeature_Callback(hObject, eventdata, handles)
% hObject    handle to btnReferenceFeature (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
handles.refIdx = FeatureNear(handles.tracks(:,:,handles.index), wrev(ginput(1)));
guidata(hObject, handles);
UpdateGUI(handles);



function textRefFeature_Callback(hObject, eventdata, handles)
% hObject    handle to textRefFeature (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of textRefFeature as text
%        str2double(get(hObject,'String')) returns contents of textRefFeature as a double


% --- Executes during object creation, after setting all properties.
function textRefFeature_CreateFcn(hObject, eventdata, handles)
% hObject    handle to textRefFeature (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in btnMovingFeature.
function btnMovingFeature_Callback(hObject, eventdata, handles)
% hObject    handle to btnMovingFeature (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
handles.movIdx = FeatureNear(handles.tracks(:,:,handles.index), wrev(ginput(1)));
guidata(hObject, handles);
UpdateGUI(handles);


function textMovFeature_Callback(hObject, eventdata, handles)
% hObject    handle to textMovFeature (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of textMovFeature as text
%        str2double(get(hObject,'String')) returns contents of textMovFeature as a double


% --- Executes during object creation, after setting all properties.
function textMovFeature_CreateFcn(hObject, eventdata, handles)
% hObject    handle to textMovFeature (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function textStartFrame_Callback(hObject, eventdata, handles)
% hObject    handle to textStartFrame (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
idx = round(str2double(get(hObject, 'String')));
idx = max(1, idx);
idx = min(idx, handles.maxIdx);
handles.startFrame = idx;
guidata(hObject, handles);
UpdateGUI(handles);

% --- Executes during object creation, after setting all properties.
function textStartFrame_CreateFcn(hObject, eventdata, handles)
% hObject    handle to textStartFrame (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function textEndFrame_Callback(hObject, eventdata, handles)
% hObject    handle to textEndFrame (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of textEndFrame as text
%        str2double(get(hObject,'String')) returns contents of textEndFrame as a double
idx = round(str2double(get(hObject, 'String')));
idx = max(1, idx);
idx = min(idx, handles.maxIdx);
handles.endFrame = idx;
guidata(hObject, handles);
UpdateGUI(handles);

% --- Executes during object creation, after setting all properties.
function textEndFrame_CreateFcn(hObject, eventdata, handles)
% hObject    handle to textEndFrame (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in btnStartNow.
function btnStartNow_Callback(hObject, eventdata, handles)
% hObject    handle to btnStartNow (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
handles.startFrame = handles.index;
guidata(hObject, handles);
UpdateGUI(handles);

% --- Executes on button press in btnEndNow.
function btnEndNow_Callback(hObject, eventdata, handles)
% hObject    handle to btnEndNow (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
handles.endFrame = handles.index;
guidata(hObject, handles);
UpdateGUI(handles);



function textStrain_Callback(hObject, eventdata, handles)
% hObject    handle to textStrain (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of textStrain as text
%        str2double(get(hObject,'String')) returns contents of textStrain as a double


% --- Executes during object creation, after setting all properties.
function textStrain_CreateFcn(hObject, eventdata, handles)
% hObject    handle to textStrain (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function textDeltaLength_Callback(hObject, eventdata, handles)
% hObject    handle to textDeltaLength (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of textDeltaLength as text
%        str2double(get(hObject,'String')) returns contents of textDeltaLength as a double


% --- Executes during object creation, after setting all properties.
function textDeltaLength_CreateFcn(hObject, eventdata, handles)
% hObject    handle to textDeltaLength (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function textLengthStart_Callback(hObject, eventdata, handles)
% hObject    handle to textLengthStart (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of textLengthStart as text
%        str2double(get(hObject,'String')) returns contents of textLengthStart as a double


% --- Executes during object creation, after setting all properties.
function textLengthStart_CreateFcn(hObject, eventdata, handles)
% hObject    handle to textLengthStart (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function textLengthEnd_Callback(hObject, eventdata, handles)
% hObject    handle to textLengthEnd (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of textLengthEnd as text
%        str2double(get(hObject,'String')) returns contents of textLengthEnd as a double


% --- Executes during object creation, after setting all properties.
function textLengthEnd_CreateFcn(hObject, eventdata, handles)
% hObject    handle to textLengthEnd (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in btnDisplayStrain.
function btnDisplayStrain_Callback(hObject, eventdata, handles)
% hObject    handle to btnDisplayStrain (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Compute strain
[S, Lo, dL, L] = ComputeFeatureStrain(handles.tracks, [handles.refIdx handles.movIdx]', [handles.startFrame handles.endFrame]');
S
dL
Lo
L

