; 脚本由 Inno Setup 脚本向导 生成！
; 有关创建 Inno Setup 脚本文件的详细资料请查阅帮助文档！

#define MyAppName "BekVideoMerge"
#define MyAppVersion "2.5.1"
#define MyAppPublisher "福州北科大舟宇电子有限公司"
#define MyAppBuildID GetDateTimeString('yyyymmdd','','');
#define MyAppURL "http://www.bekzoyo.com.cn/"
#define DotNetFile "en_.net_framework_4_full_x86_508939.exe"

[Setup]
; 注: AppId的值为单独标识该应用程序。
; 不要为其他安装程序使用相同的AppId值。
; (生成新的GUID，点击 工具|在IDE中生成GUID。)
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
;要求管理员权限
PrivilegesRequired=admin
;安装完成要求重启
AlwaysRestart=yes
;不允许卸载Uninstallable=false

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

;配置文件，如果已经存在，不覆盖
Source: "./conf/*"; DestDir: "{app}/conf"; Flags: ignoreversion recursesubdirs createallsubdirs onlyifdoesntexist

;一些通用的资源文件和配置文件
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

//控制是否跳过页面
function ShouldSkipPage(PageID: Integer): Boolean;
begin
  { initialize result to not skip any page (not necessary, but safer) }
  Result := False;
  { if the page that is asked to be skipped is your custom page, then... }
  if PageID = km2ItemTypePage.ID then
    { if the component is not selected, skip the page }
    Result := radioKm3.Checked;    //科目三不选择项目数量
end;

procedure InitializeWizard();
begin
       //考试科目类型选择页面
       kskmPage:=CreateCustomPage(wpSelectDir, '初始化设置', '设置考试科目');
       lbl:=TLabel.Create(kskmPage);
       lbl.Parent:=kskmPage.Surface;
       lbl.Caption:='请选择考试科目';

       radioKm2:=TRadioButton.Create(kskmPage);
       radioKm2.Parent:=kskmPage.Surface;
       radioKm2.Caption:='科目二';
       radioKm2.Top:=lbl.Top+30;
       radioKm2.Checked:=true;

       radioKm3:=TRadioButton.Create(kskmPage);
       radioKm3.Parent:=kskmPage.Surface;
       radioKm3.Caption:='科目三';
       radioKm3.Top:=radioKm2.Top+30;

       with kskmPage do
       begin
         OnNextButtonClick := @kskmPage_NextButtonClick;
       end;
       
       //科目二项目类型选择页面
       km2ItemTypePage:=CreateCustomPage(kskmPage.ID, '初始化设置', '设置设置科目二项目类型');
       lb2:=TLabel.Create(km2ItemTypePage);
       lb2.Parent:=km2ItemTypePage.Surface;
       lb2.Caption:='请选择科目二项目类型';

       radioItemFive:=TRadioButton.Create(km2ItemTypePage);
       radioItemFive.Parent:=km2ItemTypePage.Surface;
       radioItemFive.Caption:='标准5项';
       radioItemFive.Top:=lb2.Top+30;
       radioItemFive.Checked:=true;

       radioItemSeven:=TRadioButton.Create(km2ItemTypePage);
       radioItemSeven.Parent:=km2ItemTypePage.Surface;
       radioItemSeven.Caption:='7项（贵州）';
       radioItemSeven.Top:=radioItemFive.Top+30;

       with km2ItemTypePage do
       begin
         OnNextButtonClick := @km2ItemTypePage_NextButtonClick;
       end;

       //数据库类型选择页面
       dbPage:=CreateCustomPage(km2ItemTypePage.ID, '初始化设置', '设置数据库类型');
       lb3:=TLabel.Create(dbPage);
       lb3.Parent:=dbPage.Surface;
       lb3.Caption:='请选择数据库类型';

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

//检测服务是否已经安装
function InitializeSetup():boolean;
var
  ResultCode: Integer;
  uicmd: String;
begin
  if RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{3B83F3B3-A6F9-43A9-A90E-962EDE90E3A1}_is1', 'UninstallString', uicmd) then
  begin
    //杀进程
    Exec(ExpandConstant('{sys}\taskkill.exe'), '/im CZDemons.exe /f /t', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
    Exec(ExpandConstant('{sys}\taskkill.exe'), '/im BekVideoMerge.exe /f /t', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  end;

  Result:= True
end;

//检测系统是否已经安装.net 4.5.1
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

//安装.net 4.0
procedure InstallFramework;
var ResultCode: Integer;
begin
    Exec(ExpandConstant('{app}\3rd\vcredist.exe'), '/q /norestart', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);
    if FrameworkIsNotInstalled() then
    begin
        Exec(ExpandConstant('{tmp}\{#DotNetFile}'), '/q /norestart', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);
    end;
end;

//安装完成
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

    //将配置写入配置文件 conf/HS_CONF_ENV.ini
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

