# テキストクリップボード同期 v1

状態: **Experimental / 通信・OS連携実装済み**。既定OFF。WindowsホストとWindows Moonlight Qtを初期対象とし、別端末間の配信を通した往復と他OSのOS連携は別途検証します。画像、ファイル、HTML、空文字列によるクリアは対象外です。

## 有効化と互換性

Sunshineの`clipboard_mode`は`disabled`（既定）、`host_to_client`、`client_to_host`、`bidirectional`です。変更後にホストを再起動します。Moonlightの`Sync text clipboard`も既定OFFです。

クライアントはペアリング時の証明書で検証するHTTPSの`serverinfo`から`CustomClipboardVersion=1`と`CustomClipboardDirections`を取得します。方向のビットはホスト→クライアントが1、クライアント→ホストが2です。無効・未対応のホストはバージョン0として扱います。能力の問い合わせ失敗では同期を無効化して通常の配信を継続します。既存のテキスト入力ショートカットは維持します。

通信は認証済みセッションのAES-GCM制御経路に限定します。ホストは従来の非暗号化入力経路上のクリップボードを拒否します。ホストの購読は最初の1セッションだけに許可し、2つ目の購読・書き込みを拒否します。切断時に購読、保留内容、反射抑制状態を消去します。

## メッセージ

| 方向 | 種別 | フィールド |
| --- | --- | --- |
| クライアント→ホスト | 入力magic `0x55000008` | BE32長さ（自身4バイトを除く）、LE32 magic、LE32 token、UTF-8 |
| ホスト→クライアント | 制御type `0x5505` | LE32 token、LE32テキスト長、UTF-8 |

入力のテキスト長0は購読要求です。ホスト通知の空文字列は拒否します。tokenは不透明な識別子で、反射抑制には内容を使います。独自実験拡張であり、本流で予約・標準化された公開契約ではありません。

UTF-8の上限は32,755バイトです。上限超過は切り詰めず拒否します。不正UTF-8、過長符号化、サロゲート、埋め込みNUL、宣言長と実データの不一致を拒否します。共通の検証関数は`src/Clipboard.h`に置きます。

## OS連携と状態管理

ホスト→クライアントが許可されている場合、購読時にホストの現在のテキストを送ります。クライアント→ホストだけの場合、ホストの内容は読み取り・送信しません。空・非テキストの内容は同期せず、内容をログへ記録しません。

Windowsホストは`CF_UNICODETEXT`を厳密にUTF-8と相互変換し、250ms間隔で変更を監視します。書き込み保留は最新1件にまとめ、他アプリによる占有時は最大2秒リトライします。OSへの反映が成功した内容だけを適用済みとします。

Qtクライアントの受信保留も最新1件です。SDL通知に動的な文字列ポインタを載せず、終了時の遅延通知やリークを防ぎます。OS反映失敗は250ms間隔、最大2秒で再試行します。同じ内容の再送と反射を抑え、切断時に再試行を破棄します。

## 検証と履歴

- 共通ライブラリ: `tests/clipboard.c`。配置、ASCII、日本語、絵文字、長さ上限、不正符号化、不正パケット長。
- Sunshine: クリップボード状態・入力検証のgtestと、私用Window Station上でユーザーのクリップボードに触れず実施するWindows API連携テスト。
- Moonlight Qt: `tests/custom/clipboard.cpp`。方向、能力確認、最新通知、反射抑制、失敗・再試行、切断後の通知。

単体・OS連携テストと、別端末間の配信を通したテストを区別して互換性記録に残します。

参考実装を履歴付きで取り込み、能力確認、既定OFF、方向制御、Windows実装、境界検証、終了処理を追加しました。

- [moonlight-common-c #155](https://github.com/moonlight-stream/moonlight-common-c/pull/155)
- [moonlight-qt #2019](https://github.com/moonlight-stream/moonlight-qt/pull/2019)
- [Sunshine #5782](https://github.com/LizardByte/Sunshine/pull/5782)
