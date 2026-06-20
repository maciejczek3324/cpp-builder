object Form1: TForm1
  Left = 264
  Top = 167
  Width = 1292
  Height = 883
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object StringGrid1: TStringGrid
    Left = 64
    Top = 24
    Width = 809
    Height = 809
    ColCount = 50
    DefaultColWidth = 15
    DefaultRowHeight = 15
    FixedCols = 0
    RowCount = 50
    FixedRows = 0
    TabOrder = 0
    OnDrawCell = StringGrid1DrawCell
  end
  object ButtonTura: TButton
    Left = 1016
    Top = 616
    Width = 129
    Height = 73
    Caption = 'Przycisk ALARMOWY'
    TabOrder = 1
    OnClick = ButtonTuraClick
  end
  object MemoStats: TMemo
    Left = 992
    Top = 24
    Width = 169
    Height = 345
    Lines.Strings = (
      'MemoStats')
    ReadOnly = True
    TabOrder = 2
  end
  object Deszcz: TButton
    Left = 992
    Top = 392
    Width = 169
    Height = 57
    Caption = 'Deszcz(porost trawy)'
    TabOrder = 3
    OnClick = DeszczClick
  end
  object Antylopaplus: TButton
    Left = 888
    Top = 464
    Width = 169
    Height = 57
    Caption = 'Awaryjne dodanie antylop'
    TabOrder = 4
    OnClick = AntylopaplusClick
  end
  object Lewplus: TButton
    Left = 1088
    Top = 464
    Width = 161
    Height = 57
    Caption = 'Awaryjne dodanie lwow'
    TabOrder = 5
    OnClick = LewplusClick
  end
  object Timer1: TTimer
    Interval = 1500
    OnTimer = Timer1Timer
    Left = 1704
    Top = 80
  end
end
