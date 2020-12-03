# camApp_Dahua

## Changelog
- 0.2.0 first version. 2020-06-15@Okano
		複数Dahuaカメラ対応CamApp
		カメラプラグイン”libgstdahuasrc.so.0.2.0”にバージョンを合わせた。
		単体Dahuaカメラ対応CamAppに対して以下の機能を追加している。
		1. CamApp for TOSHIBA Teli BU1203MCでの修正を追加
			- アプリ起動時にディスプレイの中央にアプリを表示
			- Saveボタン押下で設定保存&アプリ終了
		2. アプリ実行時の引数-n追加
			- [-n Dahuaカメラの型番]
		3. アプリ実行時の引数がない場合はメイン画面の前にカメラ選択画面を表示
		4. メイン画面にツールバー(キャプチャ開始/停止、ワンショット、録画開始/停止、PNG保存、Fit)を追加

