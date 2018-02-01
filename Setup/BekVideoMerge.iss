; �ű��� Inno Setup �ű��� ���ɣ�
; �йش��� Inno Setup �ű��ļ�����ϸ��������İ����ĵ���

#define MyAppName "BekVideoMerge"
#define MyAppVersion "2.5.1"
#define MyAppPublisher "���ݱ��ƴ�����������޹�˾"
#define MyAppBuildID GetDateTimeString('yyyymmdd','','');
#define MyAppURL "http://www.bekzoyo.com.cn/"
#define DotNetFile "en_.net_framework_4_full_x86_508939.exe"

[Setup]
; ע: AppId��ֵΪ������ʶ��Ӧ�ó���
; ��ҪΪ������װ����ʹ����ͬ��AppIdֵ��
; (�����µ�GUID����� ����|��IDE������GUID��)
AppId={{3B83F3B3-A6F9-43A9-A90E-962EDE90E3A1}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
;DisableDirPage=yes
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
OutputBaseFilename={#MyAppName}_{#MyAppVersion}_Build_{#MyAppBuildID}01
Compression=lzma
SolidCompression=yes
;Ҫ�����ԱȨ��
PrivilegesRequired=admin
;��װ���Ҫ������
AlwaysRestart=yes
;������ж��Uninstallable=false

[Languages]
Name: "chinesesimp"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "./3rd-party/vcredist.exe"; DestDir: "{app}/3rdparty"; Flags: onlyifdoesntexist ignoreversionSource: "./3rd-party/{#DotNetFile}"; DestDir: "{tmp}"; Flags: onlyifdoesntexist ignoreversion deleteafterinstall; AfterInstall: InstallFramework; Check: FrameworkIsNotInstalled
Source: "./3rd-party/mencoder.exe"; DestDir: "{app}/3rdparty"; Flags: ignoreversion
Source: "./3rd-party/HKLib/*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

Source: "./common/CUdpForJMQ.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "./common/CZDemons.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "./common/CLogFilez.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "./common/ZDemons.dat"; DestDir: "{app}"; Flags: ignoreversion

Source: "./common/ConfigTool/*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

Source: "../Release/BekVideoMerge.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "../Release/CommonDll.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "../Release/BekHikUtil.dll"; DestDir: "{app}"; Flags: ignoreversion

;�����ļ�������Ѿ����ڣ�������
Source: "./conf/*"; DestDir: "{app}/conf"; Flags: ignoreversion recursesubdirs createallsubdirs onlyifdoesntexist

;һЩͨ�õ���Դ�ļ��������ļ�
Source: "./res/*"; DestDir: "{app}/res"; Flags: ignoreversion recursesubdirs createallsubdirs


[code]
var
  lbl,lb2,lb3:TLabel;
  kskmPage,km2ItemTypePage,dbPage:TwizardPage;
  radioKm2,radioKm3,radioItemFive,radioItemSeven,radioSql,radioOracle:TRadioButton;

function kskmPage_NextButtonClick(Page: TWizardPage): Boolean;
var 
  bKm2 : Boolean;
begin
  bKm2 := radioKm2.Checked;
  if bKm2 = true then
    begin 
        RegWriteStringValue(HKEY_LOCAL_MACHINE,'SOFTWARE\Bekzoyo\BekVideoMerge','KSKM','2');
        Result := true;
    end
  else
    begin
        RegWriteStringValue(HKEY_LOCAL_MACHINE,'SOFTWARE\Bekzoyo\BekVideoMerge','KSKM','3');
        Result := true;
  end
end;

function km2ItemTypePage_NextButtonClick(Page: TWizardPage): Boolean;
var 
  bItemFive : Boolean;
begin
  bItemFive := radioItemFive.Checked;
  if bItemFive = true then
    begin 
        RegWriteStringValue(HKEY_LOCAL_MACHINE,'SOFTWARE\Bekzoyo\BekVideoMerge','Km2ItemCount','5');
        Result := true;
    end
  else
    begin
        RegWriteStringValue(HKEY_LOCAL_MACHINE,'SOFTWARE\Bekzoyo\BekVideoMerge','Km2ItemCount','7');
        Result := true;
  end
end;

function dbPage_NextButtonClick(Page: TWizardPage): Boolean;
var 
  bSql : Boolean;
begin
  bSql := radioSql.Checked;
  if bSql = true then
    begin 
        RegWriteStringValue(HKEY_LOCAL_MACHINE,'SOFTWARE\Bekzoyo\BekVideoMerge','SQLORACLE','1');
        Result := true;
    end
  else
    begin
        RegWriteStringValue(HKEY_LOCAL_MACHINE,'SOFTWARE\Bekzoyo\BekVideoMerge','SQLORACLE','0');
        Result := true;
  end
end;

//�����Ƿ�����ҳ��
function ShouldSkipPage(PageID: Integer): Boolean;
begin
  { initialize result to not skip any page (not necessary, but safer) }
  Result := False;
  { if the page that is asked to be skipped is your custom page, then... }
  if PageID = km2ItemTypePage.ID then
    { if the component is not selected, skip the page }
    Result := radioKm3.Checked;    //��Ŀ����ѡ����Ŀ����
end;

procedure InitializeWizard();
begin
       //���Կ�Ŀ����ѡ��ҳ��
       kskmPage:=CreateCustomPage(wpSelectDir, '��ʼ������', '���ÿ��Կ�Ŀ');
       lbl:=TLabel.Create(kskmPage);
       lbl.Parent:=kskmPage.Surface;
       lbl.Caption:='��ѡ���Կ�Ŀ';

       radioKm2:=TRadioButton.Create(kskmPage);
       radioKm2.Parent:=kskmPage.Surface;
       radioKm2.Caption:='��Ŀ��';
       radioKm2.Top:=lbl.Top+30;
       radioKm2.Checked:=true;

       radioKm3:=TRadioButton.Create(kskmPage);
       radioKm3.Parent:=kskmPage.Surface;
       radioKm3.Caption:='��Ŀ��';
       radioKm3.Top:=radioKm2.Top+30;

       with kskmPage do
       begin
         OnNextButtonClick := @kskmPage_NextButtonClick;
       end;
       
       //��Ŀ����Ŀ����ѡ��ҳ��
       km2ItemTypePage:=CreateCustomPage(kskmPage.ID, '��ʼ������', '�������ÿ�Ŀ����Ŀ����');
       lb2:=TLabel.Create(km2ItemTypePage);
       lb2.Parent:=km2ItemTypePage.Surface;
       lb2.Caption:='��ѡ���Ŀ����Ŀ����';

       radioItemFive:=TRadioButton.Create(km2ItemTypePage);
       radioItemFive.Parent:=km2ItemTypePage.Surface;
       radioItemFive.Caption:='��׼5��';
       radioItemFive.Top:=lb2.Top+30;
       radioItemFive.Checked:=true;

       radioItemSeven:=TRadioButton.Create(km2ItemTypePage);
       radioItemSeven.Parent:=km2ItemTypePage.Surface;
       radioItemSeven.Caption:='7����ݣ�';
       radioItemSeven.Top:=radioItemFive.Top+30;

       with km2ItemTypePage do
       begin
         OnNextButtonClick := @km2ItemTypePage_NextButtonClick;
       end;

       //���ݿ�����ѡ��ҳ��
       dbPage:=CreateCustomPage(km2ItemTypePage.ID, '��ʼ������', '�������ݿ�����');
       lb3:=TLabel.Create(dbPage);
       lb3.Parent:=dbPage.Surface;
       lb3.Caption:='��ѡ�����ݿ�����';

       radioSql:=TRadioButton.Create(dbPage);
       radioSql.Parent:=dbPage.Surface;
       radioSql.Caption:='SqlServer';
       radioSql.Top:=lb3.Top+30;
       radioSql.Checked:=true;

       radioOracle:=TRadioButton.Create(dbPage);
       radioOracle.Parent:=dbPage.Surface;
       radioOracle.Caption:='Oracle';
       radioOracle.Top:=radioSql.Top+30;

       with dbPage do
       begin
         OnNextButtonClick := @dbPage_NextButtonClick;
       end;
end;

//�������Ƿ��Ѿ���װ
function InitializeSetup():boolean;
var
  ResultCode: Integer;
  uicmd: String;
begin
  if RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{3B83F3B3-A6F9-43A9-A90E-962EDE90E3A1}_is1', 'UninstallString', uicmd) then
  begin
    //ɱ����
    Exec(ExpandConstant('{sys}\taskkill.exe'), '/im CZDemons.exe /f /t', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
    Exec(ExpandConstant('{sys}\taskkill.exe'), '/im BekVideoMerge.exe /f /t', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  end;

  Result:= True
end;

//���ϵͳ�Ƿ��Ѿ���װ.net 4.5.1
function FrameworkIsNotInstalled: Boolean;
var
    bSuccess: Boolean;
    regVersion: Cardinal;
begin
    Result := True;
    bSuccess := RegQueryDWordValue(HKLM, 'Software\Microsoft\NET Framework Setup\NDP\v4\Full', 'Release', regVersion)

    if (True = bSuccess) and (regVersion >= 378675) then
    begin
        Result := False;
    end;
end;

//��װ.net 4.0
procedure InstallFramework;
var ResultCode: Integer;
begin
    Exec(ExpandConstant('{app}\3rd\vcredist.exe'), '/q /norestart', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);
    if FrameworkIsNotInstalled() then
    begin
        Exec(ExpandConstant('{tmp}\{#DotNetFile}'), '/q /norestart', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);
    end;
end;

//��װ���
procedure CurStepChanged(CurStep: TSetupStep);  
var  
uninspath, uninsname, NewUninsName, MyAppName: string;  
userName: string;
passWord: string;
parmStr:  string;
oldFilePath : string;
newfilePath : string;
resultCode: Integer;

  confPath: string;
begin  
if CurStep=ssDone then  
  begin

    //������д�������ļ� conf/HS_CONF_ENV.ini
    confPath := ExpandConstant('{app}') +'\\conf\\HS_CONF_ENV.ini';
    if radioKm2.Checked = true then begin
        SetIniString('CONFIG', 'KSKM', '2', confPath);
    end else begin
        SetIniString('CONFIG', 'KSKM', '3', confPath);
    end;
    if radioItemSeven.Checked = true then begin
        SetIniString('CONFIG', 'Km2ItemCount', '7', confPath);
    end else begin
        SetIniString('CONFIG', 'Km2ItemCount', '5', confPath);
    end;
    if radioSql.Checked = true then begin
        SetIniString('CONFIG', 'SQLORACLE', '1', confPath);
    end else begin
        SetIniString('CONFIG', 'SQLORACLE', '0', confPath);
    end;

  end;  
end;

