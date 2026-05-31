# Qt Semi-automatic translate tool

Select Language: [English](README.md) | [简体中文](README.zh-CN.md)

A simple tool to modify `*.ts` translation file for Qt. With the help of online translation, save your time.

## Usage

Step 1: Load your ts file

Step 2: Click "Export Template" button to generate a pure text file for translation. Every line in this file is the source text XXX. The XXX is `tr("XXX")` in you Qt project. It will ignore the context.

> In the template.txt, the newline will be replace by a special red emoji. You should not delete it. It will be replace to original newline when importing.

Step 3: Use your own translate tool like [Google Translate](https://translate.google.com) to process the template.txt. Load the translated.txt to the tool.

> You must keep the same line count with the row count of the dict table.

Step 4: Load your translated.txt, Click "Save TS" and overwrite the original ts file.

## Fixed dictionary

> This feature is similar to `Phrase Book` in Qt Linguist. It is a fixed string-to-string dict.

Step 1: Select multiple lines in the TS table. Click the right button of your mouse and select `Move to fixed dict`

Step 2: Edit your fixed dict on the table below. Save your json if you need.

Step 3: Save TS file.

## Qt Linguist

Some out-of-date version of Linguist could not show the correct order of `tr(xx)`, you may try [Community linguist](https://github.com/thurask/Qt-Linguist).
