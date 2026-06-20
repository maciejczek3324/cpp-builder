object Form1: TForm1
  Left = 256
  Top = 85
  Width = 1024
  Height = 675
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Button2: TButton
    Left = 24
    Top = 360
    Width = 81
    Height = 33
    Caption = 'poczatek'
    TabOrder = 0
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 24
    Top = 400
    Width = 81
    Height = 25
    Caption = 'koniec'
    TabOrder = 1
    OnClick = Button3Click
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 100
    OnTimer = Timer1Timer
    Left = 608
    Top = 112
  end
end
