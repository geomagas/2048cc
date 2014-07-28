unit FRM_Main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, Menus,
  ActnList, Grids, ComCtrls, ExtCtrls, StdCtrls, u_replay;

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
    Board: TDrawGrid;
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
    StatusBar: TStatusBar;
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
    procedure BoardDrawCell(Sender: TObject; aCol, aRow: Integer; aRect: TRect;
      aState: TGridDrawState);
    procedure FormCreate(Sender: TObject);
  private
    { private declarations }
    TheReplay:TReplay;
  public
    { public declarations }
  end;

var
  frmMain: TfrmMain;

implementation

{$R *.lfm}

{ TfrmMain }

procedure TfrmMain.actExitExecute(Sender: TObject);
begin
Close;
end;

procedure TfrmMain.actOpenExecute(Sender: TObject);
begin
With OpenDialog do
  If Execute then
  	If TheReplay.LoadFromFile(FileName) then
      With StatusBar,Board do
      	Begin
			  Invalidate; // to force redraw
			  Visible:=True;
			  SimpleText:=FileName;
			  end;
end;

procedure TfrmMain.BoardDrawCell(Sender: TObject; aCol, aRow: Integer;
  aRect: TRect; aState: TGridDrawState);
begin
With Canvas,TheReplay do
  If Dim>0 then
		TextRect(aRect,0,0,IntToStr(State^[aRow*Dim+aCol]));
end;

procedure TfrmMain.FormCreate(Sender: TObject);
begin
TheReplay:=TReplay.Create;
end;

end.

