# 共有ライブラリビルド時の静的ライブラリ組み込み機能

Shared Library Build with Static Library Embedding

## 概要 / Overview

`makelibsrc.mk` の `BUILD=shared` オプションにおいて、`LIBS` に指定された `.a` ファイル（静的ライブラリ）を共有ライブラリに自動的に静的リンクする機能が実装されています。

When building shared libraries with `BUILD=shared` in `makelibsrc.mk`, `.a` files (static libraries) specified in `LIBS` are automatically statically linked into the shared library.

## 背景・目的 / Background and Purpose

### 課題 / Problem

共有ライブラリ `libcalc.so` が静的ライブラリ `libcalcbase.a` の関数を参照する場合、以下の2つのアプローチがあります：

When a shared library `libcalc.so` references functions from a static library `libcalcbase.a`, there are two approaches:

**1. 共有ライブラリビルド時に静的リンク（推奨）**
- `libcalc.so` に `libcalcbase.a` の内容を組み込む
- 最終アプリケーションは `libcalc.so` のみをリンクすればよい
- 依存関係が共有ライブラリ内で完結

**2. 最終アプリケーションで両方をリンク**
- `libcalc.so` は未解決シンボルを含む
- 最終アプリケーションが両方のライブラリをリンク
- 配布時に複数のライブラリが必要となり管理が煩雑

**Option 1: Static linking during shared library build (recommended)**
- Embed `libcalcbase.a` contents into `libcalc.so`
- Final application only needs to link `libcalc.so`
- Dependencies are encapsulated within the shared library

**Option 2: Link both libraries in final application**
- `libcalc.so` contains unresolved symbols
- Final application must link both libraries
- Distribution becomes complex, requiring multiple libraries

### 目的 / Purpose

オプション1（推奨アプローチ）を自動的に実現します。`LIBS` に指定された静的ライブラリを共有ライブラリに組み込むことで、依存関係の管理を簡素化します。

This feature automatically implements option 1 (the recommended approach). By embedding static libraries specified in `LIBS`, it simplifies dependency management.

## 機能仕様 / Functional Specification

### 基本動作 / Basic Behavior

| LIBS の記述形式 | 処理方法 | 例 |
|----------------|---------|-----|
| 直接 `.a` 指定 | 静的リンク | `libcalcbase.a` |
| `-l` で `.a` に解決 | 静的リンク | `-lcalcbase` → `libcalcbase.a` |
| `-l` で `.so` に解決 | 動的リンク | `-lm` → `libm.so` |
| `.so` 直接指定 | 動的リンク | `libfoo.so` |
| `-L` オプション | 検索パスとして使用 | `-L/path/to/libs` |
| その他リンクオプション | そのまま渡す | `-Wl,-rpath,/path` |

| LIBS format | Processing | Example |
|-------------|------------|---------|
| Direct `.a` specification | Static linking | `libcalcbase.a` |
| `-l` resolves to `.a` | Static linking | `-lcalcbase` → `libcalcbase.a` |
| `-l` resolves to `.so` | Dynamic linking | `-lm` → `libm.so` |
| Direct `.so` specification | Dynamic linking | `libfoo.so` |
| `-L` option | Used as search path | `-L/path/to/libs` |
| Other link options | Passed as-is | `-Wl,-rpath,/path` |

### ライブラリ検索パス / Library Search Path

`-l` オプションで指定されたライブラリは、以下の順序で `.a` ファイルを検索します：

Libraries specified with `-l` are searched for `.a` files in the following order:

1. `LIBSDIR` で指定されたパス
2. `LIBS` 内の `-L` オプションで指定されたパス
3. `$(WORKSPACE_FOLDER)/test/lib`
4. システムライブラリパス (`/usr/lib`, `/usr/local/lib`, `/lib`, etc.)

1. Paths specified in `LIBSDIR`
2. Paths specified with `-L` option in `LIBS`
3. `$(WORKSPACE_FOLDER)/test/lib`
4. System library paths (`/usr/lib`, `/usr/local/lib`, `/lib`, etc.)

最初に見つかった `.a` ファイルが使用されます。見つからない場合は `-l` オプションとして動的リンクされます。

The first found `.a` file is used. If not found, it remains as `-l` option for dynamic linking.

### 動作フロー / Operation Flow

```
BUILD=shared の場合:
  ↓
LIBS から .a ファイルを直接抽出
  ↓
LIBS から -l オプションを抽出して検索パスから .a を検索
  ├─ .a が見つかる → 静的リンク対象
  └─ .a が見つからない → 動的リンク対象
  ↓
共有ライブラリをビルド:
  gcc -shared -o libXXX.so (オブジェクト) (静的ライブラリ) (動的ライブラリオプション)
```

## 使用例 / Usage Examples

### 例1: LIBSDIR を使用 / Example 1: Using LIBSDIR

```makefile
# Makefile
LIBSDIR = $(WORKSPACE_FOLDER)/prod/calc/lib
LIBS = -lcalcbase -lm
BUILD = shared
```

**動作:**
- `libcalcbase.a` が `prod/calc/lib` に存在 → 静的リンク
- `-lm` → 動的リンク

**結果:**
```bash
gcc -shared -o libcalc.so obj/*.o /path/to/libcalcbase.a -lm
```

**Operation:**
- `libcalcbase.a` found in `prod/calc/lib` → static linking
- `-lm` → dynamic linking

### 例2: -L オプションを使用 / Example 2: Using -L option

```makefile
# Makefile
LIBS = -L$(WORKSPACE_FOLDER)/test/lib -lcalcbase -lm
BUILD = shared
```

**動作:**
- `libcalcbase.a` が `test/lib` に存在 → 静的リンク
- `-lm` → 動的リンク
- `-L` オプションもリンクコマンドに渡される

**結果:**
```bash
gcc -shared -o libcalc.so obj/*.o /path/to/libcalcbase.a -lm -L/path/to/test/lib
```

**Operation:**
- `libcalcbase.a` found in `test/lib` → static linking
- `-lm` → dynamic linking
- `-L` option is also passed to link command

### 例3: 直接 .a を指定 / Example 3: Direct .a specification

```makefile
# Makefile
LIBS = libcalcbase.a -lm
BUILD = shared
```

**動作:**
- `libcalcbase.a` を直接指定 → 静的リンク
- `-lm` → 動的リンク

**結果:**
```bash
gcc -shared -o libcalc.so obj/*.o libcalcbase.a -lm
```

**Operation:**
- `libcalcbase.a` directly specified → static linking
- `-lm` → dynamic linking

### 例4: 混在パターン / Example 4: Mixed pattern

```makefile
# Makefile
LIBS = libbase.a -L/custom/path -lcustom -lpthread -Wl,-rpath,/opt/lib
BUILD = shared
```

**動作:**
- `libbase.a` → 静的リンク（直接指定）
- `-lcustom` → `/custom/path` に `.a` があれば静的リンク、なければ動的リンク
- `-lpthread` → 動的リンク（通常 `.so` のみ存在）
- `-Wl,-rpath,/opt/lib` → リンクオプションとしてそのまま渡される

**Operation:**
- `libbase.a` → static linking (direct specification)
- `-lcustom` → static linking if `.a` exists in `/custom/path`, otherwise dynamic
- `-lpthread` → dynamic linking (usually only `.so` exists)
- `-Wl,-rpath,/opt/lib` → passed as-is

## 依存関係の自動更新 / Automatic Dependency Updates

静的ライブラリが更新された場合、共有ライブラリも自動的に再ビルドされます。

When a static library is updated, the shared library is automatically rebuilt.

**例:**
1. `libcalcbase.a` を変更
2. `make` を実行
3. `libcalc.so` が自動的に再ビルドされる

**Example:**
1. Modify `libcalcbase.a`
2. Run `make`
3. `libcalc.so` is automatically rebuilt

## 制限事項 / Limitations

### 1. 検索対象は `.a` のみ

`.a` ファイルのみを検索します。同名の `.so` と `.a` が存在する場合でも、`.a` が優先されます。

Only `.a` files are searched. Even if both `.so` and `.a` exist with the same name, `.a` is prioritized.

### 2. 検索パスの順序

最初に見つかった `.a` ファイルが使用されます。複数のパスに同名のライブラリがある場合は、検索パスの順序（`LIBSDIR` → `-L` → `test/lib` → システムパス）に注意してください。

The first found `.a` file is used. When multiple paths contain libraries with the same name, be aware of the search order (`LIBSDIR` → `-L` → `test/lib` → system paths).

### 3. アーキテクチャ依存パス

x86_64 Linux 用のシステムライブラリパス（`/usr/lib/x86_64-linux-gnu` など）がハードコードされています。他のアーキテクチャでは、`makelibsrc.mk` の `ALL_LIB_DIRS` に適切なパスを追加する必要があります。

System library paths for x86_64 Linux (`/usr/lib/x86_64-linux-gnu`, etc.) are hardcoded. For other architectures, appropriate paths must be added to `ALL_LIB_DIRS` in `makelibsrc.mk`.

### 4. BUILD=static には影響なし

この機能は `BUILD=shared` の場合のみ有効です。`BUILD=static`（デフォルト）の動作は変更されていません。

This feature is only active when `BUILD=shared`. The behavior of `BUILD=static` (default) is unchanged.

## 実装ファイル / Implementation File

**ファイル**: `makefw/makefiles/makelibsrc.mk`

**変更行**: 162-210行目（共有ライブラリビルドルール部分）

**File**: `makefw/makefiles/makelibsrc.mk`

**Modified lines**: Lines 162-210 (shared library build rules)

詳細な実装内容については、ソースコード内のコメントを参照してください。

For detailed implementation, refer to comments in the source code.

## 参考 / References

- **リンカーの挙動**: `ld` は `.a` ファイルを直接指定すると静的リンクし、`-l` オプションは検索パスから `.so` または `.a` を探します
- **依存関係管理**: Make の自動依存関係解決機能により、静的ライブラリの更新を検知します

- **Linker behavior**: `ld` statically links when `.a` files are directly specified, while `-l` options search for `.so` or `.a` in search paths
- **Dependency management**: Make's automatic dependency resolution detects static library updates
