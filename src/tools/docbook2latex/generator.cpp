#include "generator.h"
#include "preambule.tex.h"

#include "docbookviewer/docbookmodel.h"

#include <QTextCodec>
#include <QByteArray>
#include <QDir>
#include <QMap>

namespace docbook2latex {

using namespace std;
using namespace DocBookViewer;

generator::generator()
    : addSpace_(false)
    , tableColumns_(0u)
    , twoColumnLayout_(false)
    , allowRasterImages_(false)
{
}


generator * generator::self()
{
    static generator * instance = new generator();
    return instance;
}

void generator::saveToFile(const std::string &filename) const
{
    const QString qFileName = QString::fromStdString(filename);
    QFile f(qFileName);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextCodec * koi8 = QTextCodec::codecForName("KOI8-R");
        f.write(koi8->fromUnicode(data_));
        f.close();
    }
    else {
        throw string("Can't open file for write: ") + filename;
    }
}

bool generator::isEmpty() const
{
    return ! bool(root_);
}

void generator::reset()
{
    root_.clear();
    data_.clear();

}


void generator::renderData(ModelPtr data)
{
    root_ = data;
    QString preambule = QString::fromUtf8(PREAMBULE);
    QString paper, clazz, clazzparam, tmargin, bmargin, lmargin, rmargin, omargin, imargin;
    const QString profile = QString::fromStdString(options::profile).toLower();
    static const QStringList ValidProfiles = QStringList() << "a4" << "a5" <<
                                                              "a4ts" << "a5ts" <<
                                                              "tablet";
    if (!ValidProfiles.contains(profile)) {
        throw string("Not valid profile: ") + options::profile;
    }
    if (profile.startsWith("a4")) {
        paper = "a4paper";
        tmargin = "2cm";
        bmargin = "2cm";
        imargin = "2cm";
        omargin = "3cm";
    }
    else if (profile != "tablet") {
        paper = "a5paper";
        tmargin = "2cm";
        bmargin = "2cm";
        imargin = "2cm";
        omargin = "3cm";
    }
    else {
        paper = "a5paper";
        tmargin = "0.1cm";
        bmargin = "0.1cm";
        imargin = "0.5cm";
        omargin = "0.5cm";
        allowRasterImages_ = true;
    }
    clazzparam = paper;
    if (profile.endsWith("ts")) {
        clazzparam += ",twoside";
        lmargin = imargin;
        rmargin = omargin;
    }
    else {
        clazzparam += ",oneside";
        lmargin = imargin;
        rmargin = omargin;
    }
    clazz = root_ == DocBookModel::Book ? "book" : "article";

    preambule.replace("%%paper%%", paper);
    preambule.replace("%%class%%", clazz);
    preambule.replace("%%classparam%%", clazzparam);
    preambule.replace("%%tmargin%%", tmargin);
    preambule.replace("%%bmargin%%", bmargin);
    preambule.replace("%%lmargin%%", lmargin);
    preambule.replace("%%rmargin%%", rmargin);
    data_ = preambule + "\n";
    data_ += "\\begin{document}\n\n";
    if (clazz == "article" && paper == "a4paper") {
//        twoColumnLayout_ = true;
    }
    data_ += DOCUMENT_BEGIN;
    if (root_ == DocBookModel::Book) {
        data_ += renderBookTitle();
    }
    else {
        data_ += renderArticleTitle();
    }
    Q_FOREACH(ModelPtr child, data->children()) {
        data_ += renderElement(child);
    }

    data_.replace("--\\", "-- \\");
    data_.replace(":\\", ": \\");
    data_.replace("}(", "} (");
    data_ += "\n\\end{document}\n";
}

QString generator::renderElement(ModelPtr data)
{
    QString result;
    if (data == DocBookModel::Chapter) {
        result = renderChapter(data);
    }
    else if (data == DocBookModel::Section) {
        result = renderSection(data);
    }
    else if (data == DocBookModel::Para) {
        result = renderPara(data);
    }
    else if (data == DocBookModel::Emphasis) {
        result = renderEmphasis(data);
    }
    else if (data == DocBookModel::Text) {
        if (data->parent() == DocBookModel::ProgramListing ||
                data->parent() == DocBookModel::Code)
        {
            result = data->text().trimmed();
            static QMap<QString,QString> replacements;
            if (replacements.empty()) {
                replacements["~"] = "\\~";
                replacements["&lt;"] = "<";
                replacements["&gt;"] = ">";
//                replacements["_"] = "\\_";
                replacements[QString(QChar(0x2264))] = "$<=$";
                replacements[QString(QChar(0x2265))] = "$>=$";
            }
            foreach (const QString & key, replacements.keys())
            {
                result.replace(key, replacements[key]);
            }
            addSpace_ = data->parent() == DocBookModel::Code;
        }
        else {
            QString normText = normalizeText(data->text());
            if (addSpace_ && normText.length() > 0 &&
                    ( normText[0].isLetterOrNumber() || normText[0] == '-' )
                    )
                result += " ";
            result += normText;
            addSpace_ = result.length() > 0 &&
                    result.at(result.length()-1).isLetterOrNumber();
        }
    }
    else if (data == DocBookModel::OrderedList) {
        result = renderOrderedList(data);
    }
    else if (data == DocBookModel::ItemizedList) {
        result = renderItemizedList(data);
    }
    else if (data == DocBookModel::ListItem) {
        result = renderListItem(data);
    }
    else if (data == DocBookModel::ProgramListing) {
        result = renderProgramListing(data);
    }
    else if (data == DocBookModel::Code) {
        result = renderCode(data);
    }
    else if (data == DocBookModel::Example) {
        result = renderExample(data);
    }
    else if (data == DocBookModel::Table) {
        result = renderTable(data);
    }
    else if (data == DocBookModel::Subscript) {
        result = renderSubscript(data);
    }
    else if (data == DocBookModel::Superscript) {
        result = renderSuperscript(data);
    }
    else if (data == DocBookModel::MathML_Math) {
        result = renderMath(data);
    }
    else if (data == DocBookModel::MathML_MRow) {
        result = renderMRow(data);
    }
    else if (data == DocBookModel::MathML_MI) {
        result = renderMI(data);
    }
    else if (data == DocBookModel::MathML_MN) {
        result = renderMN(data);
    }
    else if (data == DocBookModel::MathML_MO) {
        result = renderMO(data);
    }
    else if (data == DocBookModel::MathML_MSup) {
        result = renderMSup(data);
    }
    else if (data == DocBookModel::MathML_MSqrt) {
        result = renderMSqrt(data);
    }
    else if (data == DocBookModel::MathML_MFrac) {
        result = renderMFrac(data);
    }
    else if (data == DocBookModel::FuncSynopsys) {
        result = renderFuncSynopsis(data);
    }
    else if (data == DocBookModel::FuncPrototype) {
        result = renderFuncPrototype(data);
    }
    else if (data == DocBookModel::FuncDef) {
        result = renderFuncDef(data);
    }
    else if (data == DocBookModel::Function) {
        result = renderFunction(data);
    }
    else if (data == DocBookModel::ParamDef) {
        result = renderParamDef(data);
    }
    else if (data == DocBookModel::Parameter) {
        result = renderParameter(data);
    }
    else if (data == DocBookModel::Type) {
        result = renderType(data);
    }
    else if (data == DocBookModel::MediaObject) {
        result = renderMediaObject(data);
    }
    return result;
}

QString generator::renderChapter(ModelPtr data)
{
    QString result;
    result = QString::fromAscii("\\chapter{%1}\n").arg(normalizeText(data->title()));
    result += "\\thispagestyle{fancy}\n";
    result += "\\setcounter{Example}{0}\n";
    result += "\\setcounter{Figure}{0}\n";
    result += "\\setcounter{Table}{0}\n";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\\vfill\n";
    return result;
}


QString generator::renderSection(ModelPtr data)
{
    QString result;
    QString sec =
            (root_ == DocBookModel::Article && data->sectionLevel() == 1) ||
            (root_ == DocBookModel::Book && data->sectionLevel() == 1)
            ? "section" : "subsection";


    result = QString::fromAscii("\\%1{%2}\n\n").arg(sec).arg(normalizeText(data->title()));

    if (sec == "section" && twoColumnLayout_) {
        result = QString::fromAscii("\\begin{multicols}{2}[%1]\n").arg(result.trimmed());
    }

    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }

    if (sec == "section" && twoColumnLayout_) {
        result += QString::fromAscii("\\end{multicols}\n");
        result += sectionFloats_.join("\n");
        sectionFloats_.clear();
    }

    return result;
}


QString generator::renderPara(ModelPtr data)
{
    QString result = "\n\n";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\n\n";
    return result;
}

QString generator::renderFuncSynopsis(ModelPtr data)
{
    QString result = "\\begin{FuncSynopsis}\n";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\\end{FuncSynopsis}\n";
    return result;
}

QString generator::renderFuncPrototype(ModelPtr data)
{
    QString result;
    ModelPtr funcDef;
    QList<ModelPtr> paramDefs;
    Q_FOREACH(ModelPtr child, data->children()) {
        if (child == DocBookModel::FuncDef)
            funcDef = child;
        else if (child == DocBookModel::ParamDef)
            paramDefs.push_back(child);
    }
    if (funcDef) {
        result += renderElement(funcDef);
    }
    if (paramDefs.size() > 0) {
        result += "(";
        addSpace_ = false;
        for (int i=0; i<paramDefs.size(); i++) {
            if (i>0)
                result += ", ";
            result += renderElement(paramDefs[i]);
        }
        result += ")";
    }
    return result;
}

QString generator::renderFuncDef(ModelPtr data)
{
    QString result = "\\textbf{";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderParamDef(ModelPtr data)
{
    QString result = "\\textbf{";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderFunction(ModelPtr data)
{
    QString result = "\\normalfont~";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    return result;
}

QString generator::renderParameter(ModelPtr data)
{
    QString result = "\\normalfont\\textit{";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderType(ModelPtr data)
{
    QString result = "\\textbf{";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderSubscript(ModelPtr data)
{
    QString result = "\\textsubscript{";
    addSpace_ = false;
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderSuperscript(ModelPtr data)
{
    QString result = "\\textsuperscript{";
    addSpace_ = false;
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderMath(ModelPtr data)
{
    addSpace_ = false;
    QString result;
    if (data->children().size() > 1) {
        result += "\begin{equation}\n";
    }
    else {
        result += " $";
    }
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    if (data->children().size() > 1) {
        result += "\end{equation}\n";
    }
    else {
        result += "$ ";
    }
    if (result == " $$ ") {
        addSpace_ = true;
        return "";
    }
    else {
        addSpace_ = false;
        return result;
    }
}

QString generator::renderMRow(ModelPtr data)
{
    QString result;
    addSpace_ = false;
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    return result;
}

QString generator::renderMSup(ModelPtr data)
{
    QString result = "^{";
    addSpace_ = false;
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderMSqrt(ModelPtr data)
{
    QString result = "\\sqrt{";
    addSpace_ = false;
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderMFrac(ModelPtr data)
{
    QString above, below;
    QList<ModelPtr> mrows;
    Q_FOREACH(ModelPtr child, data->children()) {
        if (child == DocBookModel::MathML_MRow) {
            mrows << child;
        }
    }
    if (mrows.size() > 1) {
        above = renderElement(mrows[0]);
        below = renderElement(mrows[1]);
    }
    return QString::fromAscii("\\frac{%1}{%2}").arg(above).arg(below);
}

QString generator::renderMN(ModelPtr data)
{
    QString result = "\\mathrm{";
    addSpace_ = false;
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderMO(ModelPtr data)
{
    QString result;
    addSpace_ = false;
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    return result;
}


QString generator::renderMI(ModelPtr data)
{
    QString name = data->children().size() > 0 ?
                data->children().at(0)->text().trimmed() : "";
    static const QStringList PredefinedNames = QStringList()
            << "sin" << "cos" << "tan" << "cot" << "arcsin"
            << "arccos" << "arctan" << "arccot" << "sinh"
            << "cosh" << "tanh" << "coth" << "sec" << "csc";

    static const QStringList RMNames = QStringList()
            << "ln" << "log" << "lg" << "tg" << "ctg" <<
               "arctg" << "arcctg";

    static QMap<QChar,QString> GreekSymbols;
    if (GreekSymbols.isEmpty()) {
        GreekSymbols[QChar(0x03B1)] = "alpha";
        GreekSymbols[QChar(0x03B2)] = "beta";
        GreekSymbols[QChar(0x03B3)] = "gamma";
    }

    QString result;
    if (PredefinedNames.contains(name))
        result = "\\" + name;
    else if (RMNames.contains(name))
        result = "\\mathrm{" + name + "}";
    else if (name.length() == 1 && GreekSymbols.contains(name.at(0)))
        result = "\\" + GreekSymbols[name.at(0)];
    else
        result = name;
    addSpace_ = false;
    return result;
}


QString generator::normalizeText(QString textData) const
{
    static QMap<QString,QString> replacements;
    if (replacements.empty()) {
        replacements["~"] = "\\~";
        replacements["_"] = "\\_";
        replacements[QString(QChar(0x2264))] = "$<=$";
        replacements[QString(QChar(0x2265))] = "$>=$";
        replacements[QString(QChar(0x00D7))] = "$\\times$";
        replacements[QString(QChar(0x2026))] = "$\\ldots$";
    }
    foreach (const QString & key, replacements.keys())
    {
        textData.replace(key, replacements[key]);
    }
    bool inQuote = false;
    for (int i=0; i<textData.length(); i++) {
        if (textData[i] == '"') {
            if (!inQuote) {
                textData.replace(i, 1, "<<");
                inQuote = true;
            }
            else {
                textData.replace(i, 1, ">>");
                inQuote = false;
            }
        }
    }
    return textData.simplified();
}

QString generator::renderProgramListing(ModelPtr data)
{
    QString result;
    result +=
            "\\lstset{"
            "   aboveskip=0pt,"
            "   belowskip=0pt";
    if (data->role().length() > 0) {
        result += ",language=" + data->role();
    }
    result += "}\n";
    result += "\\begin{lstlisting}\n";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\\end{lstlisting}\n";
    return result;
}

QString generator::renderCode(ModelPtr data)
{
    QString result;

    result += " ";

    result +=
            "\\lstset{"
            "   aboveskip=0pt,"
            "   belowskip=0pt";
    if (data->role().length() > 0) {
        result += ",language=" + data->role();
    }
    result += "}";
    result += "\\lstinline{";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderExample(ModelPtr data)
{
    QString result;
    result += QString::fromAscii("\\begin{Example}\n");
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += QString::fromAscii("\\caption{%1}\n").arg(normalizeText(data->title()));
    result += "\\end{Example}\n";
    if (twoColumnLayout_) {
        sectionFloats_.push_back(result);
        return "";
    }
    else {
        return result;
    }
}

QString generator::renderMediaObject(ModelPtr data)
{
    QString result;    
    ModelPtr caption;
    QString imageUrl;
    Q_FOREACH(ModelPtr child, data->children()) {
        if (child == DocBookModel::Caption)
            caption = child;
        else if (child == DocBookModel::ImageObject && imageUrl.length()==0) {
            if (child->children().size() > 0 &&
                    child->children().at(0) == DocBookModel::ImageData)
            {
                ModelPtr imageData = child->children().at(0);
                imageUrl = imageData->href().toLocalFile();
                if (!imageUrl.toLower().endsWith(".pdf"))
                    imageUrl.clear();
            }
        }
    }
    if (imageUrl.length() == 0 && allowRasterImages_) {
        Q_FOREACH(ModelPtr child, data->children()) {
            if (child == DocBookModel::ImageObject && imageUrl.length()==0) {
                if (child->children().size() > 0 &&
                        child->children().at(0) == DocBookModel::ImageData)
                {
                    ModelPtr imageData = child->children().at(0);
                    imageUrl = imageData->href().toLocalFile();
                    if (!imageUrl.toLower().endsWith(".png"))
                        imageUrl.clear();
                }
            }
        }
    }
    if (imageUrl.length() == 0)
        return "";
    result += QString::fromAscii("\\begin{Figure}\n");
    result += "\\centering\\includegraphics{" + imageUrl + "}\n";
    result += "\\caption{";
    if (caption) {
        Q_FOREACH(ModelPtr child, caption->children())
            result += renderElement(child);
    }
    result += "}\n";
    result += "\\end{Figure}\n";
    if (twoColumnLayout_) {
        sectionFloats_.push_back(result);
        return "";
    }
    else {
        return result;
    }
    return result;
}

QString generator::renderTable(ModelPtr data)
{
    QString result;
    result += QString::fromAscii("\\begin{Table}\n");
    result += QString::fromAscii("\\caption{%1}\n").arg(normalizeText(data->title()));
    tableColumns_ = 0u;
    calculateTableColumnsCount(data);
    result += "\\begin{mdframed}\n";
    result += QString::fromAscii("\\begin{tabularx}{\\linewidth}{");
    for (size_t c=0u; c<tableColumns_; c++)
        result += "*{7}{>{\\centering\\arraybackslash}X}";
    result += "}\n\\hline\n";
    ModelPtr head;
    ModelPtr body;
    Q_FOREACH(ModelPtr child, data->children()) {
        if (child == DocBookModel::THead)
            head = child;
        else if (child == DocBookModel::TBody)
            body = child;
    }
    if (head && head->children().count() > 0 && head->children().at(0) == DocBookModel::Row) {
        result += renderTableRow(head->children().at(0));
        result += " \\\\\n\\hline\n";
    }
    Q_FOREACH(ModelPtr row, body->children()) {
        if (row == DocBookModel::Row) {
            result += renderTableRow(row);
            result += " \\\\[2ex]\n";
        }
    }
    result += "\\end{tabularx}\n";
    result += "\\end{mdframed}\n";
    result += "\\end{Table}\n";
    if (twoColumnLayout_) {
        sectionFloats_.push_back(result);
        return "";
    }
    else {
        return result;
    }
    return result;
}

QString generator::renderTableRow(ModelPtr data)
{
    QString result;
    int index = 0;
    addSpace_ = false;
    Q_FOREACH(ModelPtr col, data->children()) {
        if (col == DocBookModel::Entry) {
            QString entry;
            Q_FOREACH(ModelPtr content, col->children()) {
                entry += renderElement(content);
            }
            entry = entry.trimmed();
            if (entry.length() == 0)
                result += "~";
            else
                result += "{" + entry + "}";
            if (index < data->children().size()-1)
                result += " & ";
            index ++;
        }
    }

    return result;
}

void generator::calculateTableColumnsCount(ModelPtr data)
{
    Q_FOREACH(ModelPtr child, data->children()) {
        if (child == DocBookModel::Row)
            tableColumns_ = max(tableColumns_, calculateRowColumnsCount(child));
        else if (child == DocBookModel::THead || child == DocBookModel::TBody)
            calculateTableColumnsCount(child);
    }
}

size_t generator::calculateRowColumnsCount(ModelPtr data)
{
    size_t result = 0u;
    Q_FOREACH(ModelPtr child, data->children()) {
        if (child == DocBookModel::Entry) {
            result += 1u;
        }
    }
    return result;
}

QString generator::renderEmphasis(ModelPtr data)
{
    QString result;
    if (data->role() == "bold")
        result += "\\textbf{";
    else
        result += "\\textit{";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "}";
    return result;
}

QString generator::renderBookTitle()
{
    QString result;
    result += QString::fromAscii("\\title{%1}\n").arg(root_->title());
    result += "\\maketitle\n";
    return result;
}

QString generator::renderArticleTitle()
{
    QString result;
    result += QString::fromAscii("\\title{%1}\n").arg(root_->title());
    result += "\\thispagestyle{fancy}\n";

//    result += "\\twocolumn[\\begin{@twocolumnfalse}\n";

    result += "\\maketitle\n";
    ModelPtr abstract;
    Q_FOREACH(ModelPtr child, root_->children()) {
        if (child == DocBookModel::Abstract || child == DocBookModel::Preface)
            abstract = child;
    }
    if (abstract) {
        result += "\\begin{Abstract}\n";
        Q_FOREACH(ModelPtr achild, abstract->children()) result += renderElement(achild);
        result += "\\end{Abstract}\n";
    }
//    result += "\\end{@twocolumnfalse}\n]\n";
    return result;
}

QString generator::renderOrderedList(ModelPtr data)
{
    QString result;
    result += "\\begin{enumerate}\n";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\\end{enumerate}\n";
    return result;
}

QString generator::renderItemizedList(ModelPtr data)
{
    QString result;
    result += "\\begin{itemize}\n";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\\end{itemize}\n";
    return result;
}

QString generator::renderListItem(ModelPtr data)
{
    QString result;
    result += "\\item ";
    Q_FOREACH(ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    result += "\n";
    return result;
}


} // namespace docbook2pdf