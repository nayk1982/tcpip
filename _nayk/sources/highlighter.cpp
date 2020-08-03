/****************************************************************************
** Copyright (c) 2019 Evgeny Teterin (nayk) <sutcedortal@gmail.com>
** All right reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/
#include <QTextCharFormat>
#include <QBrush>
#include <QColor>
#include <QRegularExpression>

#include "highlighter.h"

namespace nayk { //=============================================================

Highlighter::Highlighter(QTextDocument *parent, HighlightSyntax syntax, bool darkBackground)
    : QSyntaxHighlighter(parent)
    , m_syntax(syntax)
{
    HighlightingRule rule;

    edgeTagFormat.setForeground(darkBackground
                                ? QBrush(QColor("#35aaee"))
                                : QBrush(QColor("#2090ba")) );
    insideTagFormat.setForeground(darkBackground
                                  ? QBrush(QColor("#3055f0"))
                                  : Qt::blue);
    insideTagFormat.setFontWeight(QFont::Bold);
    openTag = QRegularExpression("<");
    closeTag = QRegularExpression(">");

    tagsFormat.setForeground(darkBackground ? Qt::cyan : Qt::darkBlue);
    tagsFormat.setFontWeight(QFont::Bold);

    keywordFormat.setForeground( darkBackground ? Qt::cyan : Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;

    switch (m_syntax) {
    case SyntaxHTML:
        keywordPatterns << "<\\ba\\b" << "<\\babbr\\b" << "<\\bacronym\\b" << "<\\baddress\\b" << "<\\bapplet\\b"
                            << "<\\barea\\b" << "<\\barticle\\b" << "<\\baside\\b" << "<\\baudio\\b" << "<\\bb\\b"
                            << "<\\bbase\\b" << "<\\bbasefont\\b" << "<\\bbdi\\b" << "<\\bbdo\\b" << "<\\bbgsound\\b"
                            << "<\\bblockquote\\b" << "<\\bbig\\b" << "<\\bbody\\b" << "<\\bblink\\b" << "<\\bbr\\b"
                            << "<\\bbutton\\b" << "<\\bcanvas\\b" << "<\\bcaption\\b" << "<\\bcenter\\b" << "<\\bcite\\b"
                            << "<\\bcode\\b" << "<\\bcol\\b" << "<\\bcolgroup\\b" << "<\\bcommand\\b" << "<\\bcomment\\b"
                            << "<\\bdata\\b" << "<\\bdatalist\\b" << "<\\bdd\\b" << "<\\bdel\\b" << "<\\bdetails\\b"
                            << "<\\bdfn\\b" << "<\\bdialog\\b" << "<\\bdir\\b" << "<\\bdiv\\b" << "<\\bdl\\b"
                            << "<\\bdt\\b" << "<\\bem\\b" << "<\\bembed\\b" << "<\\bfieldset\\b" << "<\\bfigcaption\\b"
                            << "<\\bfigure\\b" << "<\\bfont\\b" << "<\\bfooter\\b" << "<\\bform\\b" << "<\\bframe\\b"
                            << "<\\bframeset\\b" << "<\\bh1\\b" << "<\\bh2\\b" << "<\\bh3\\b" << "<\\bh4\\b"
                            << "<\\bh5\\b" << "<\\bh6\\b" << "<\\bhead\\b" << "<\\bheader\\b" << "<\\bhgroup\\b"
                            << "<\\bhr\\b" << "<\\bhtml\\b" << "<\\bi\\b" << "<\\biframe\\b" << "<\\bimg\\b"
                            << "<\\binput\\b" << "<\\bins\\b" << "<\\bisindex\\b" << "<\\bkbd\\b" << "<\\bkeygen\\b"
                            << "<\\blabel\\b" << "<\\blegend\\b" << "<\\bli\\b" << "<\\blink\\b" << "<\\blisting\\b"
                            << "<\\bmain\\b" << "<\\bmap\\b" << "<\\bmarquee\\b" << "<\\bmark\\b" << "<\\bmenu\\b"
                            << "<\\bamenuitem\\b" << "<\\bmeta\\b" << "<\\bmeter\\b" << "<\\bmulticol\\b" << "<\\bnav\\b"
                            << "<\\bnobr\\b" << "<\\bnoembed\\b" << "<\\bnoindex\\b" << "<\\bnoframes\\b" << "<\\bnoscript\\b"
                            << "<\\bobject\\b" << "<\\bol\\b" << "<\\boptgroup\\b" << "<\\boption\\b" << "<\\boutput\\b"
                            << "<\\bp\\b" << "<\\bparam\\b" << "<\\bpicture\\b" << "<\\bplaintext\\b" << "<\\bpre\\b"
                            << "<\\bprogress\\b" << "<\\bq\\b" << "<\\brp\\b" << "<\\brt\\b" << "<\\brtc\\b" << "<\\bruby\\b"
                            << "<\\bs\\b" << "<\\bsamp\\b" << "<\\bscript\\b" << "<\\bsection\\b" << "<\\bselect\\b"
                            << "<\\bsmall\\b" << "<\\bsource\\b" << "<\\bspacer\\b" << "<\\bspan\\b" << "<\\bstrike\\b"
                            << "<\\bstrong\\b" << "<\\bstyle\\b" << "<\\bsub\\b" << "<\\bsummary\\b" << "<\\bsup\\b"
                            << "<\\btable\\b" << "<\\btbody\\b" << "<\\btd\\b" << "<\\btemplate\\b" << "<\\btextarea\\b"
                            << "<\\btfoot\\b" << "<\\bth\\b" << "<\\bthead\\b" << "<\\btime\\b" << "<\\btitle\\b"
                            << "<\\btr\\b" << "<\\btrack\\b" << "<\\btt\\b" << "<\\bu\\b" << "<\\bul\\b" << "<\\bvar\\b"
                            << "<\\bvideo\\b" << "<\\bwbr\\b" << "<\\bxmp\\b" << "<\\bi\\b";
        break;
    case SyntaxCPP:
        keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                        << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                        << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                        << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                        << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                        << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                        << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                        << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                        << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                        << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bbool\\b";
        break;
    case SyntaxSQL:
        keywordPatterns << "\\bselect\\b" << "\\bSELECT\\b" << "\\binsert\\b" << "\\bINSERT\\b"
                        << "\\balter\\b" << "\\bALTER\\b" << "\\bis\\b" << "\\bIS\\b" << "\\create\\b" << "\\bCREATE\\b"
                        << "\\badd\\b" << "\\bADD\\b" << "\\bcolumn\\b" << "\\bCOLUMN\\b"
                        << "\\btable\\b" << "\\bTABLE\\b" << "\\bdrop\\b" << "\\bDROP\\b" << "\\bon\\b" << "\\bON\\b"
                        << "\\border\\b" << "\\bORDER\\b" << "\\bshow\\b" << "\\bSHOW\\b"
                        << "\\bdatabase\\b" << "\\bDATABASE\\b" << "\\bdatabases\\b" << "\\bDATABASES\\b"
                        << "\\binner\\b" << "\\bINNER\\b" << "\\bdublicate\\b" << "\\bDUBLICATE\\b"
                        << "\\bupdate\\b" << "\\bUPDATE\\b" << "\\bdelete\\b" << "\\bDELETE\\b"
                        << "\\btruncate\\b" << "\\bTRUNCATE\\b" << "\\bfrom\\b" << "\\bFROM\\b"<< "\\bas\\b" << "\\bAS\\b"
                        << "\\bwhere\\b" << "\\bWHERE\\b" << "\\bset\\b" << "\\bSET\\b" << "\\bvalues\\b" << "\\bVALUES\\b"
                        << "\\bcount\\b" << "\\bCOUNT\\b" << "\\binto\\b" << "\\bINTO\\b" << "\\blike\\b" << "\\bLIKE\\b"
                        << "\\bin\\b" << "\\bIN\\b" << "\\band\\b" << "\\bAND\\b" << "\\bor\\b" << "\\bOR\\b" << "\\bnot\\b" << "\\bNOT\\b"
                        << "\\bbetween\\b" << "\\bBETWEEN\\b" << "\\breturning\\b" << "\\bRETURNING\\b"
                        << "\\bby\\b" << "\\bBY\\b" << "\\basc\\b" << "\\bASC\\b" << "\\bdesc\\b" << "\\bDESC\\b"
                        << "\\bleft\\b" << "\\bLEFT\\b" << "\\bright\\b" << "\\bRIGHT\\b" << "\\bjoin\\b" << "\\bJOIN\\b"
                        << "\\blimit\\b" << "\\bLIMIT\\b" << "\\bouter\\b" << "\\bOUTER\\b";
        break;
    }

    for (const QString &pattern : keywordPatterns) {

        rule.pattern = QRegularExpression(pattern);
        if(m_syntax == SyntaxHTML) {
            rule.format = tagsFormat;
            startTagRules.append(rule);
        }
        else {
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }
    }

    if(m_syntax == SyntaxHTML) {

        QStringList keywordPatterns_end;
        keywordPatterns_end << "<!\\bDOCTYPE\\b" << "</\\ba\\b" << "</\\babbr\\b" << "</\\bacronym\\b" << "</\\baddress\\b" << "</\\bapplet\\b"
                            << "</\\barea\\b" << "</\\barticle\\b" << "</\\baside\\b" << "</\\baudio\\b" << "</\\bb\\b"
                            << "</\\bbase\\b" << "</\\bbasefont\\b" << "</\\bbdi\\b" << "</\\bbdo\\b" << "</\\bbgsound\\b"
                            << "</\\bblockquote\\b" << "</\\bbig\\b" << "</\\bbody\\b" << "</\\bblink\\b" << "</\\bbr\\b"
                            << "</\\bbutton\\b" << "</\\bcanvas\\b" << "</\\bcaption\\b" << "</\\bcenter\\b" << "</\\bcite\\b"
                            << "</\\bcode\\b" << "</\\bcol\\b" << "</\\bcolgroup\\b" << "</\\bcommand\\b" << "</\\bcomment\\b"
                            << "</\\bdata\\b" << "</\\bdatalist\\b" << "</\\bdd\\b" << "</\\bdel\\b" << "</\\bdetails\\b"
                            << "</\\bdfn\\b" << "</\\bdialog\\b" << "</\\bdir\\b" << "</\\bdiv\\b" << "</\\bdl\\b"
                            << "</\\bdt\\b" << "</\\bem\\b" << "</\\bembed\\b" << "</\\bfieldset\\b" << "</\\bfigcaption\\b"
                            << "</\\bfigure\\b" << "</\\bfont\\b" << "</\\bfooter\\b" << "</\\bform\\b" << "</\\bframe\\b"
                            << "</\\bframeset\\b" << "</\\bh1\\b" << "</\\bh2\\b" << "</\\bh3\\b" << "</\\bh4\\b"
                            << "</\\bh5\\b" << "</\\bh6\\b" << "</\\bhead\\b" << "</\\bheader\\b" << "</\\bhgroup\\b"
                            << "</\\bhr\\b" << "</\\bhtml\\b" << "</\\bi\\b" << "</\\biframe\\b" << "</\\bimg\\b"
                            << "</\\binput\\b" << "</\\bins\\b" << "</\\bisindex\\b" << "</\\bkbd\\b" << "</\\bkeygen\\b"
                            << "</\\blabel\\b" << "</\\blegend\\b" << "</\\bli\\b" << "</\\blink\\b" << "</\\blisting\\b"
                            << "</\\bmain\\b" << "</\\bmap\\b" << "</\\bmarquee\\b" << "</\\bmark\\b" << "</\\bmenu\\b"
                            << "</\\bamenuitem\\b" << "</\\bmeta\\b" << "</\\bmeter\\b" << "</\\bmulticol\\b" << "</\\bnav\\b"
                            << "</\\bnobr\\b" << "</\\bnoembed\\b" << "</\\bnoindex\\b" << "</\\bnoframes\\b" << "</\\bnoscript\\b"
                            << "</\\bobject\\b" << "</\\bol\\b" << "</\\boptgroup\\b" << "</\\boption\\b" << "</\\boutput\\b"
                            << "</\\bp\\b" << "</\\bparam\\b" << "</\\bpicture\\b" << "</\\bplaintext\\b" << "</\\bpre\\b"
                            << "</\\bprogress\\b" << "</\\bq\\b" << "</\\brp\\b" << "</\\brt\\b" << "</\\brtc\\b" << "</\\bruby\\b"
                            << "</\\bs\\b" << "</\\bsamp\\b" << "</\\bscript\\b" << "</\\bsection\\b" << "</\\bselect\\b"
                            << "</\\bsmall\\b" << "</\\bsource\\b" << "</\\bspacer\\b" << "</\\bspan\\b" << "</\\bstrike\\b"
                            << "</\\bstrong\\b" << "</\\bstyle\\b" << "</\\bsub\\b" << "</\\bsummary\\b" << "</\\bsup\\b"
                            << "</\\btable\\b" << "</\\btbody\\b" << "</\\btd\\b" << "</\\btemplate\\b" << "</\\btextarea\\b"
                            << "</\\btfoot\\b" << "</\\bth\\b" << "</\\bthead\\b" << "</\\btime\\b" << "</\\btitle\\b"
                            << "</\\btr\\b" << "</\\btrack\\b" << "</\\btt\\b" << "</\\bu\\b" << "</\\bul\\b" << "</\\bvar\\b"
                            << "</\\bvideo\\b" << "</\\bwbr\\b" << "</\\bxmp\\b" << "</\\bi\\b";

        for (const QString &pattern : keywordPatterns_end) {
            rule.pattern = QRegularExpression(pattern);
            rule.format = tagsFormat;
            endTagRules.append(rule);
        }

    }

    multiLineCommentFormat.setForeground(Qt::darkGray);
    if(m_syntax == SyntaxHTML) {
        commentStartExpression = QRegularExpression("<!--");
        commentEndExpression = QRegularExpression("-->");
    }
    else {
        commentStartExpression = QRegularExpression("/\\*");
        commentEndExpression = QRegularExpression("\\*/");
    }

    quotationFormat.setForeground(darkBackground ? Qt::green : Qt::darkGreen);
    quotes = QRegularExpression("\"");

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(darkBackground ? Qt::magenta : Qt::darkMagenta);
    singleLineCommentFormat.setForeground(Qt::darkGray);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);

    if(m_syntax == SyntaxSQL) {

        rule.pattern = QRegularExpression("::[A-Za-z0-9_]*");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        rule.pattern = QRegularExpression("[\\+\\-\\=\\>\\<\\;\\.\\,]");
        rule.format = classFormat;
        highlightingRules.append(rule);

    }
    else if(m_syntax == SyntaxCPP) {

        rule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        rule.pattern = QRegularExpression("//[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        rule.pattern = QRegularExpression("\".*\"");
        rule.format = quotationFormat;
        highlightingRules.append(rule);
    }

    if(m_syntax != SyntaxHTML) {
        rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }
}
//==============================================================================
void Highlighter::highlightBlock(const QString &text)
{
    if(m_syntax == SyntaxHTML) {
        highlightBlockHtml(text);
        return;
    }

    for (const HighlightingRule &rule: highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {

        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
//==============================================================================
void Highlighter::highlightBlockHtml(const QString &text)
{
    setCurrentBlockState(None);

    int startIndex = 0;

    if (previousBlockState() != Tag && previousBlockState() != Quote) {
        startIndex = openTag.match(text).capturedStart();
    }

    int subPreviousTag = previousBlockState();

    while (startIndex >= 0) {

        QRegularExpressionMatch match = closeTag.match(text, startIndex);
        int endIndex = match.capturedStart();

        int tagLength;

        if (endIndex == -1) {
            setCurrentBlockState(Tag);
            tagLength = text.length() - startIndex;
        }
        else {
            tagLength = endIndex - startIndex + match.capturedLength();
        }

        if (subPreviousTag != Tag) {
            setFormat(startIndex, 1, edgeTagFormat);
            setFormat(startIndex + 1, tagLength - 1, insideTagFormat);
        }
        else {
            setFormat(startIndex, tagLength, insideTagFormat);
            subPreviousTag = None;
        }

        setFormat(endIndex, 1, edgeTagFormat);
        int startQuoteIndex = 0;

        if (previousBlockState() != Quote) {
            startQuoteIndex = quotes.match(text, startIndex).capturedStart();
        }

        while (startQuoteIndex >= 0
               && ((startQuoteIndex < endIndex) || (endIndex == -1))) {

            QRegularExpressionMatch match1 = quotes.match(text, startQuoteIndex + 1);
            int endQuoteIndex = match1.capturedStart();
            int quoteLength;

            if (endQuoteIndex == -1) {
                setCurrentBlockState(Quote);
                quoteLength = text.length() - startQuoteIndex;
            }
            else {
                quoteLength = endQuoteIndex - startQuoteIndex + match1.capturedLength();
            }

            if ((endIndex > endQuoteIndex) || endIndex == -1) {
                setFormat(startQuoteIndex, quoteLength, quotationFormat);
                startQuoteIndex = quotes.match(text, startQuoteIndex + quoteLength).capturedStart();
            }
            else {
                break;
            }
        }

        startIndex = openTag.match(text, startIndex + tagLength).capturedStart();
    }

    for (const HighlightingRule &rule : startTagRules) {

        QRegularExpression expression(rule.pattern);
        QRegularExpressionMatch match = expression.match(text);
        int index = match.capturedStart();

        while (index >= 0) {
            int length = match.capturedLength();
            setFormat(index + 1, length - 1, rule.format);
            match = expression.match(text, index + length);
            index = match.capturedStart();
        }
    }

    for (const HighlightingRule &rule : endTagRules) {

        QRegularExpression expression(rule.pattern);
        QRegularExpressionMatch match = expression.match(text);
        int index = match.capturedStart();
        while (index >= 0) {
            int length = match.capturedLength();
            setFormat(index + 1, 1, edgeTagFormat);
            setFormat(index + 2, length - 2, rule.format);
            match = expression.match(text, index + length);
            index = match.capturedStart();
        }
    }

    int startCommentIndex = 0;

    if (previousBlockState() != Comment) {
        startCommentIndex = commentStartExpression.match(text).capturedStart();
    }

    while (startCommentIndex >= 0) {

        QRegularExpressionMatch match = commentEndExpression.match(text, startCommentIndex);
        int endCommentIndex = match.capturedStart();
        int commentLength;

        if (endCommentIndex == -1) {
            setCurrentBlockState(Comment);
            commentLength = text.length() - startCommentIndex;
        }
        else  {
            commentLength = endCommentIndex - startCommentIndex
                            + match.capturedLength();
        }

        setFormat(startCommentIndex, commentLength, multiLineCommentFormat);
        startCommentIndex = commentStartExpression.match(text, startCommentIndex
                                                         + commentLength).capturedStart();
    }
}

} // namespace nayk //==========================================================
