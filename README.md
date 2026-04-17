# Qt半自动翻译

第一步，从ts文件生成txt文件，该纯文本每行内容，是`tr("xx")`中的xx，并且忽略上下文

```bash
./app --ts app.ts --txt src.txt
```

> 该纯文本中，每一个回车符会替换成一个红色的emoji，注意保留该特殊符号，最终会进行逆操作替换回原回车符。

第二步，将该纯文本拖入翻译软件，如Google Translate Web，翻译后得到translated.txt

第三步，应用翻译

```bash
./app --ts app.ts --txt translated.txt --ts-out app_translated.ts
```

# 语言家

Qt5的老旧版本，没有按tr()顺序显示词语，可以使用较新的版本。如网友[单独编译的语言家](https://github.com/thurask/Qt-Linguist)
