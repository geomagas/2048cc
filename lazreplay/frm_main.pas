unit FRM_Main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, Menus,
  ActnList, Grids, ComCtrls, ExtCtrls, StdCtrls;

type

  { TfrmMain }

  TfrmMain = class(TForm)
    actExit: TAction;
    actFirst: TAction;
    actLast: TAction;
    actNext: TAction;
    actPrev: TAction;
    actPause: TAction;
    actPlay: TAction;
    actOpen: TAction;
    ActionList: TActionList;
    DrawGrid1: TDrawGrid;
    ActionImageList: TImageList;
    Image1: TImage;
    Image2: TImage;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    MenuItem10: TMenuItem;
    MenuItem11: TMenuItem;
    MenuItem12: TMenuItem;
    MenuItem4: TMenuItem;
    MenuItem5: TMenuItem;
    MenuItem6: TMenuItem;
    MenuItem7: TMenuItem;
    MenuItem8: TMenuItem;
    MenuItem9: TMenuItem;
    MoveImageList: TImageList;
    MainMenu: TMainMenu;
    MenuItem1: TMenuItem;
    MenuItem2: TMenuItem;
    MenuItem3: TMenuItem;
    OpenDialog: TOpenDialog;
    StatusBar1: TStatusBar;
    ToolBar1: TToolBar;
    ToolButton1: TToolButton;
    ToolButton2: TToolButton;
    ToolButton4: TToolButton;
    ToolButton5: TToolButton;
    ToolButton6: TToolButton;
    ToolButton7: TToolButton;
    ToolButton8: TToolButton;
    procedure actExitExecute(Sender: TObject);
    procedure actOpenExecute(Sender: TObject);
  private
    { private declarations }
    Procedure LoadReplay(FileName:String);
  public
    { public declarations }
  end;

var
  frmMain: TfrmMain;

implementation

{$R *.lfm}

{ TfrmMain }

Procedure TfrmMain.LoadReplay(FileName:String);
Begin
ShowMessage('Suppose '+FileName+' is loaded.');
End;

procedure TfrmMain.actExitExecute(Sender: TObject);
begin
Close;
end;

procedure TfrmMain.actOpenExecute(Sender: TObject);
begin
With OpenDialog do If Execute then LoadReplay(FileName);
end;

end.

