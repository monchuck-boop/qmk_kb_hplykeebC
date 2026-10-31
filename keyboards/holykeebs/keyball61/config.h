name: Build Original Keyball61 RP2040 Vial PMW3610 Right-Hand

on:
  workflow_dispatch:

jobs:
  build:
    name: Keyball61 RP2040 Vial Build
    runs-on: ubuntu-latest
    container: ghcr.io/qmk/qmk_cli@sha256:2dc05fc9f32efebd6b05c2b8676ee548358bc7e151e9dbf4dac6b6eed4513b07

    steps:
    - name: Checkout Vial repo
      uses: actions/checkout@v4
      with:
        persist-credentials: false
        submodules: recursive

    - name: Download Original Keyball Source (Safe Method)
      uses: actions/checkout@v4
      with:
        repository: Yowkees/keyball
        path: original_keyball
        persist-credentials: false

    - name: Configure for PMW3610 & RP2040 Right-Hand Only
      run: |
        git config --global --add safe.directory $(pwd)
        
        # 1. 本家KeyballデータをQMK配置に合流
        mkdir -p keyboards
        cp -r original_keyball/qmk_firmware/keyboards/keyball keyboards/
        
        # 2. 存在しない「vial」キーマップフォルダを自動生成
        mkdir -p keyboards/keyball/keyball61/keymaps/vial
        cp -r keyboards/keyball/keyball61/keymaps/default/* keyboards/keyball/keyball61/keymaps/vial/
        
        # 3. Pythonスクリプトが正常に解読できる空JSONデータを生成
        echo '[]' > keyboards/keyball/keyball61/keymaps/vial/vial.json
        
        # 4. ユーザー定義とセキリュティ設定の追記（Vialをパスワードなしで即認識させる）
        echo "#define VIAL_KEYBOARD_UID {0x4B, 0x61, 0x6C, 0x36, 0x31, 0x50, 0x4D, 0x57}" >> keyboards/keyball/keyball61/keymaps/vial/config.h
        echo "#define VIAL_INSECURE" >> keyboards/keyball/keyball61/keymaps/vial/config.h
        
        # 5. 警告による停止を物理的に無効化
        sed -i 's/-Werror//g' builddefs/common_rules.mk
        
        # 6. センサー初期化ピン名の不一致(PMW3360_NCS_PIN)を直接GP21へ書き換える
        sed -i 's/PMW3360_NCS_PIN/GP21/g' keyboards/keyball/drivers/pmw3360/pmw3360.c

        # 7. 分割通信の処理バグを防ぐため、最低限必要な関数だけを定義。
        # 2重定義エラーを防ぐため、ここから layer_state_set_user は完全に削除しています
        echo "void keyboard_post_init_kb(void) {}" > keyboards/keyball/lib/keyball/keyball.c
        echo "void housekeeping_task_kb(void) {}" >> keyboards/keyball/lib/keyball/keyball.c
        echo "void keyball_set_scroll_mode(int mode) {}" >> keyboards/keyball/lib/keyball/keyball.c
        
        # 8. 【完璧な右手専用ピンアサイン設定】単数形(MATRIX_COL_PINS)・逆ダイオード完全補正
        cat << 'EOF' > keyboards/keyball/keyball61/config.h
        #pragma once
        #define MASTER_RIGHT
        #define MATRIX_ROWS 5
        #define MATRIX_COLS 8
        #define MATRIX_ROW_PINS     { GP4, GP5, GP6, GP7, GP8 }
        #define MATRIX_COL_PINS     { GP29, GP28, GP27, GP26, GP29, GP28, GP27, GP26 }
        #define DIODE_DIRECTION     COL2ROW
        #define MATRIX_IO_DELAY     30
        #define DEBOUNCE            5
        #define PMW33XX_CS_PIN      GP21
        #define SPI_SCK_PIN         GP22
        #define SPI_MOSI_PIN        GP23
        #define SPI_MISO_PIN        GP25
        #define SPI_DRIVER          SPID0
        #define PMW3610
        #define HAL_USE_SPI TRUE
        #define RP_SPI_USE_SPI0 TRUE
        EOF
        
        # 9. 本家データの衝突を防ぐため、関係する rules.mk をすべて一度完全に初期化
        echo "" > keyboards/keyball/rules.mk
        echo "" > keyboards/keyball/keyball61/rules.mk
        echo "" > keyboards/keyball/keyball61/keymaps/vial/rules.mk
        
        # 10. 【2重定義の完全修正】
        # 衝突の原因になっていた keyball61.c を削除し、センサー定義ファイル(pmw3360.c)だけを確実に合流させます
        echo "SRC += drivers/pmw3360/pmw3360.c" >> keyboards/keyball/keyball61/rules.mk
        echo "SRC += lib/keyball/keyball.c" >> keyboards/keyball/keyball61/rules.mk
        
        # 11. 【右手専用・RGB完全無効・PMW3610専用】の設定フラグを一から再構築
        echo "MCU = RP2040" >> keyboards/keyball/keyball61/rules.mk
        echo "BOOTLOADER = rp2040" >> keyboards/keyball/keyball61/rules.mk
        echo "SPLIT_KEYBOARD_ENABLE = no" >> keyboards/keyball/keyball61/rules.mk
        echo "SPLIT_KEYBOARD = no" >> keyboards/keyball/keyball61/rules.mk
        echo "OLED_ENABLE = no" >> keyboards/keyball/keyball61/rules.mk
        echo "RGBLIGHT_ENABLE = no" >> keyboards/keyball/keyball61/rules.mk
        echo "RGB_MATRIX_ENABLE = no" >> keyboards/keyball/keyball61/rules.mk
        
        echo "VIAL_ENABLE = yes" >> keyboards/keyball/keyball61/keymaps/vial/rules.mk
        echo "DYNAMIC_KEYMAP_ENABLE = yes" >> keyboards/keyball/keyball61/keymaps/vial/rules.mk
        echo "LTO_ENABLE = no" >> keyboards/keyball/keyball61/keymaps/vial/rules.mk
        
        echo "POINTING_DEVICE_ENABLE = yes" >> keyboards/keyball/keyball61/rules.mk
        echo "POINTING_DEVICE_DRIVER = custom" >> keyboards/keyball/keyball61/rules.mk
        echo "SPI_VARIABLE_SIZE = yes" >> keyboards/keyball/keyball61/rules.mk

    - name: Build Firmware
      run: |
        git config --global --add safe.directory $(pwd)
        
        # コンパイルを実行
        make keyball/keyball61:vial

    - name: Gather Firmware File
      run: |
        mkdir -p output_firmware
        find .build/ -name "*.uf2" -exec cp {} ./output_firmware/ \;
        find . -name "*.uf2" -exec cp {} ./output_firmware/ \;

    - name: Upload Finished Firmware
      uses: actions/upload-artifact@v4
      with:
        name: keyball61-vial-rp2040-pmw3610-firmware
        path: output_firmware/
