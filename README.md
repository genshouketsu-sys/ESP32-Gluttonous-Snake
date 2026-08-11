# ESP32-Gluttonous-Snake

[中文](#中文) | [English](#english) | [日本語](#日本語)

---

## 中文

基于 ESP32 打造的局域网分布式多人贪吃蛇硬件服务器。
将 ESP32 作为一个高速的数据转发中枢，把游戏渲染和物理运算全部分散给玩家手机端执行，实现局域网内低延迟的高速电竞体验。

### 特性
- **分布式状态同步**: ESP32 仅充当 WebSocket 路由器，玩家各自计算蛇身坐标并广播，最大化压榨硬件性能。
- **强制 Captive Portal**: 通电即生成 AP 热点，手机连上自动弹出网页游戏界面，纯纯的街机体验。
- **高可用匹配机制**: 支持 1~99 号房间。可以 `AUTO MATCH` 自动找空位，也可以手动输入房间号观战或参战。
- **动态身份流转**: 提供 玩家/观战者 动态切换功能。
- **百分子局**: 率先拿到 100 分 (吃 10 个苹果) 的玩家强制冻结画面并触发胜者全网通报。
- **单机模式**: 想一个人练手？直接进 `SINGLE PLAYER`，本地运算，跟服务器零交互。

### 游玩规则
1. 手机连上热点 `ESP32-Web-Server` (默认密码: `11111111`)。
2. 在自动弹出的 Captive Portal 页面中输入你的名字 (ID)。
3. 点击 `[ AUTO MATCH ]` 自动进入空余房间，或手动输入房间号后 `JOIN`。
4. **控制**: 游戏只有 `<- TURN` 和 `TURN ->` 两个按钮。控制蛇头相对当前方向的左转和右转，专门为手机大拇指操控优化。
5. **胜负判定**: 先吃满 10 个方块到达 100 分的人赢。
6. **耻辱柱/名人堂**: 胜者 ID 会被推送到所有连接了该热点的手机底部，直接霸屏，直到 ESP32 断电重启。

### 部署说明 (PlatformIO)
本项目基于 PlatformIO 构建，请确保已安装 VSCode + PlatformIO 插件。
1. 用 PlatformIO 打开工程目录 `ESP32_WebServer`。
2. 依赖库已经写在 `platformio.ini` 里，会自动拉取。
3. **烧录后端固件**: 插上 ESP32，点击左下角的 `Upload` 箭头 (或终端执行 `pio run -t upload`)。
4. **烧录前端页面 (LittleFS)**: 点击左侧边栏 PlatformIO 图标，执行 `Upload Filesystem Image` (或终端执行 `pio run -t uploadfs`) 把 `data` 目录下的网页写进闪存。
5. **离线独立运行**: 烧录完成后，插在一个普通的充电宝上，这块板子就是一个独立的实体街机厅了。

---

## English

A LAN-based distributed multiplayer Snake hardware server powered by ESP32.
The ESP32 acts merely as a high-speed data relay hub, offloading all game rendering and physics calculations to the players' smartphones, achieving low-latency esports-level experience over local Wi-Fi.

### Features
- **Distributed State Sync**: ESP32 acts purely as a WebSocket router. Each player calculates their own snake's coordinates and broadcasts them, maximizing hardware efficiency.
- **Captive Portal**: Generates an AP hotspot immediately upon power-up. Connect with a phone and the web game interface pops up automatically.
- **Auto Matchmaking**: Supports rooms 1~99. Use `AUTO MATCH` to find an empty slot, or manually enter a room number to play or spectate.
- **Dynamic Roles**: Switch between Player and Spectator dynamically.
- **100-Point Rule**: The first player to reach 100 points (eat 10 apples) freezes the screen and triggers a global winner announcement.
- **Single Player**: Want to practice alone? Hit `SINGLE PLAYER` for local execution with zero server interaction.

### How to Play
1. Connect to Wi-Fi `ESP32-Web-Server` (Password: `11111111`).
2. Enter your Name/ID in the Captive Portal page.
3. Click `[ AUTO MATCH ]` or enter a room number and click `JOIN`.
4. **Controls**: Only two buttons: `<- TURN` and `TURN ->`. They steer the snake left/right relative to its current direction (optimized for thumbs).
5. **Win Condition**: Eat 10 apples to hit 100 points and win instantly.
6. **Hall of Fame**: The winner's ID is pushed to all devices connected to the ESP32 hotspot until it reboots.

### Deployment (PlatformIO)
This project is built using PlatformIO. Please ensure you have VSCode + PlatformIO installed.
1. Open the `ESP32_WebServer` directory in PlatformIO.
2. Dependencies are defined in `platformio.ini` and will be pulled automatically.
3. **Flash Firmware (Backend)**: Connect the ESP32 and click `Upload` (or run `pio run -t upload`).
4. **Flash Filesystem (Frontend)**: Open the PlatformIO sidebar, go to Project Tasks, and execute `Upload Filesystem Image` (or run `pio run -t uploadfs`) to write the HTML files to flash memory.
5. **Standalone Operation**: Once flashed, plug the ESP32 into a power bank, and it becomes an independent arcade server.

---

## 日本語

ESP32を搭載したローカルネットワーク分散型マルチプレイヤー「スネーク」ハードウェアサーバー。
ESP32を高速データ中継ハブとして機能させ、ゲームのレンダリングと物理演算をすべて各プレイヤーのスマートフォンにオフロードすることで、低遅延のLAN対戦を実現します。

### 特徴
- **分散型ステート同期**: ESP32はWebSocketルーターとしてのみ機能。各プレイヤーが自分のヘビの座標を計算してブロードキャストし、ハードウェア性能を極限まで引き出します。
- **キャプティブポータル**: 電源を入れると即座にAPホットスポットを生成。スマホで接続するだけで自動的にゲーム画面がポップアップします。
- **自動マッチング**: 1〜99号室をサポート。`AUTO MATCH` で空きを探すか、手動でルーム番号を入力して観戦・参加が可能です。
- **動的ロール切り替え**: プレイヤーと観戦者の役割を動的に切り替える機能。
- **100点先取ルール**: 先に100ポイント（リンゴ10個）を獲得したプレイヤーが画面をフリーズさせ、全員に勝者としてアナウンスされます。
- **シングルプレイヤー**: サーバーと通信せず、ローカル環境のみで動作する一人用練習モードを搭載。

### 遊び方
1. Wi-Fi `ESP32-Web-Server` に接続します（パスワード: `11111111`）。
2. 自動的に表示される画面で名前（ID）を入力します。
3. `[ AUTO MATCH ]` をクリックするか、ルーム番号を入力して `JOIN` を押します。
4. **操作**: ボタンは `<- TURN` と `TURN ->` のみ。現在の進行方向に対して左右に曲がります（親指操作に最適化）。
5. **勝利条件**: 先にリンゴを10個食べて100点に到達した人が勝ちです。
6. **殿堂入り**: 勝者のIDは、ESP32の電源が切れるまで、接続している全スマホの画面下部に表示され続けます。

### デプロイ方法 (PlatformIO)
このプロジェクトはPlatformIOで構築されています。VSCode + PlatformIOがインストールされていることを確認してください。
1. PlatformIOで `ESP32_WebServer` ディレクトリを開きます。
2. 依存ライブラリは `platformio.ini` に記載されており、自動で取得されます。
3. **バックエンドの書き込み**: ESP32を接続し、`Upload` をクリックします（または `pio run -t upload` を実行）。
4. **フロントエンド (LittleFS) の書き込み**: PlatformIOのサイドバーから `Upload Filesystem Image` を実行し（または `pio run -t uploadfs`）、`data`ディレクトリ内のHTMLをフラッシュメモリに書き込みます。
5. **スタンドアロン動作**: 書き込み後、モバイルバッテリーに挿すだけで、独立したアーケードサーバーとして機能します。
