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
  private
    { private declarations }
    Procedure RefreshBoard;
    Procedure LoadReplay(FileName:String);
  public
    { public declarations }
  end;

var
  frmMain: TfrmMain;

implementation

Type
	TBoard=Array of Integer;
  pBoard=^TBoard;
	TState=Record
    win,prev,next:Integer;
    board:pBoard;
    End;
	pState=^TState;

Function CreateState(win,prev,next:Integer;board:pBoard):pState;
Begin
Result:=new(pState);
Result^.win:=win;
Result^.prev:=prev;
Result^.next:=next;
Result^.board:=board;
End;

Function BoardFromString(s:String):pBoard;
Var
	sl:TStringList;
  i:Integer;
Begin
sl:=TStringList.Create;
sl.Delimiter:=' ';
sl.DelimitedText:=s;
SetLength(Result^,sl.Count);
For i:=0 to sl.Count-1 do
	Result^[i]:=StrToInt(sl.Strings[i]);
sl.Free;
end;

{$R *.lfm}

{ TfrmMain }

Procedure TfrmMain.RefreshBoard;
Var
	r,c:Integer;
  state:pBoard;
Begin
state:=BoardFromString('0 0 0 0 1 0 0 2 0 4 0 0 3 0 0 0');
With Board do
  For r:=0 to RowCount-1 do
    For c:=0 to ColCount-1 do
    	Canvas.TextRect(CellRect(c,r),0,0,IntToStr(state^[r*4+c]));
end;

Procedure TfrmMain.LoadReplay(FileName:String);
Var
	sav:TStringList;
Begin
sav:=TStringList.Create;
With sav do
	Begin
  LoadFromFile(FileName);
  End;
Board.Visible:=True;
StatusBar.SimpleText:=FileName;
RefreshBoard;
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

