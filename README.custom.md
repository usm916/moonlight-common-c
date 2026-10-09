# Moonlight Common Custom

Sunshine CustomとMoonlight Qt Customで使用する共通通信ライブラリのフォークです。

- 本流: [moonlight-stream/moonlight-common-c](https://github.com/moonlight-stream/moonlight-common-c)
- 保存先: [usm916/moonlight-common-c](https://github.com/usm916/moonlight-common-c)
- 本流追跡: `master`（`upstream/master`）。独自変更の統合: `custom/main`。
- 初期ベース: `f900dd4767759c7b9d0e93bcea666b55c69ea62f`。
- ホスト: [usm916/Sunshine](https://github.com/usm916/Sunshine/tree/custom/main)
- クライアント: [usm916/moonlight-qt](https://github.com/usm916/moonlight-qt/tree/custom/main)

`feature/clipboard-text`にテキスト同期の通信実装を追加しました。[共通仕様](docs/custom/clipboard-v1.md)を両側の基準とします。実験機能として既定OFFで提供します。

## 変更と取り込み

独自機能は`custom/main`から`feature/clipboard-text`などを作って実装・検証し、統合します。共通ライブラリを更新したら、SunshineとMoonlight Qtのsubmoduleを同じ固定コミットへ更新し、互換性記録も更新します。

本流更新は作業ツリーを空にしてから、各コマンドの成功を確認しながら取り込みます。

```powershell
git fetch upstream --tags
git switch master
git merge --ff-only upstream/master
git switch custom/main
git switch -c integration/upstream-YYYYMMDD
git merge --no-ff upstream/master
git submodule sync --recursive
git submodule update --init --recursive
# ビルド・共通テスト・ホスト/クライアントの接続確認
git switch custom/main
git merge --ff-only integration/upstream-YYYYMMDD
git push origin custom/main
```

`YYYYMMDD`は実施日へ置き換えます。公開済みの統合ブランチをrebaseしません。ENetとnanorsは本流が固定したコミットを使い、`submodule update --remote`で無条件に変更しません。

ホストとクライアントで同じ共通ライブラリを使っていても、Sunshineのサーバー実装がライブラリのクライアント処理をそのまま利用するわけではありません。メッセージ定義、検証ルール、テスト用データを共通化し、実際のクリップボード取得・反映とサーバー側の送受信は各プロジェクトで担当します。

