#include "ast.h"
#include "ast_type.h"
#include "ast_module.h"
#include "ast_algorhitm.h"
#include "lexem.h"

#include <QtCore>

namespace AST {

Module::Module()
{
    builtInID = 0;
    header.type = ModTypeUser;
    header.enabled = false;
}

Module::Module( const struct Module * src )
{
    header.enabled = src->header.enabled;
    for (int i=0; i<src->impl.globals.size(); i++) {
        impl.globals << new Variable(src->impl.globals[i]);
    }

    for (int i=0; i<src->impl.algorhitms.size(); i++) {
        impl.algorhitms << new Algorhitm(src->impl.algorhitms[i]);
    }

    for (int i=0; i<src->impl.initializerBody.size(); i++) {
        impl.initializerBody << new Statement(src->impl.initializerBody[i]);
    }

    header.type = src->header.type;
    header.name = src->header.name;
    header.uses = src->header.uses;
    header.types = src->header.types;

}

void Module::updateReferences(const Module *src, const Data *srcData, const Data *data)
{
    for (int i=0; i<impl.globals.size(); i++) {
        impl.globals[i]->updateReferences(src->impl.globals[i], srcData, data);
    }
    for (int i=0; i<impl.algorhitms.size(); i++) {
        impl.algorhitms[i]->updateReferences(src->impl.algorhitms[i], srcData, data);
    }
    for (int i=0; i<impl.initializerBody.size(); i++) {
        impl.initializerBody[i]->updateReferences(src->impl.initializerBody[i], srcData, data);
    }
    if (header.type==ModTypeExternal) {
        header.algorhitms = src->header.algorhitms;
    }
    else {
        for (int i=0; i<src->header.algorhitms.size(); i++) {
            struct Algorhitm * alg = NULL;
            for (int j=0; j<src->impl.algorhitms.size(); j++) {
                if (src->impl.algorhitms[j]==src->header.algorhitms[i]) {
                    alg = src->impl.algorhitms[j];
                    break;
                }
            }
            Q_CHECK_PTR(alg);
            header.algorhitms << alg;
        }
    }
}

Module::~Module()
{
    for (int i=0; i<impl.algorhitms.size(); i++) {
        if (impl.algorhitms[i]) {
            int header_index = header.algorhitms.indexOf(impl.algorhitms[i]);
            if (header_index!=-1)
                header.algorhitms[header_index] = 0;
            delete impl.algorhitms[i];
            impl.algorhitms[i] = 0;
        }
    }
    for (int i=0; i<header.algorhitms.size(); i++) {
        if (header.algorhitms[i]) {
            delete header.algorhitms[i];
            header.algorhitms[i] = 0;
        }
    }
    for (int i=0; i<impl.globals.size(); i++) {
        if (impl.globals[i]) {
            delete impl.globals[i];
            impl.globals[i] = 0;
        }
    }
    for (int i=0; i<impl.initializerBody.size(); i++) {
        if (impl.initializerBody[i]) {
            delete impl.initializerBody[i];
            impl.initializerBody[i] = 0;
        }
    }
}

extern QString addIndent(const QString & source, int count)
{
    QStringList lines = source.trimmed().split("\n");
    QString result;
    for (int i=0; i<lines.size(); i++) {
        for (int j=0; j<count; j++) {
            result += "\t";
        }
        result += lines[i];
        if (i<lines.size()-1) {
            result += "\n";
        }
    }
    return result;
}

extern QString screenString(QString s);

extern QString dumpLexem(const struct Lexem *lx)
{
    QString result = "{ ";
    result += "line: "+QString::number(lx->lineNo);
    result += ", pos: "+QString::number(lx->linePos);
    result += ", len: "+QString::number(lx->length);
    if (!lx->error.isEmpty()) {
        result += ", error: \""+screenString(lx->error)+"\"";
    }
    result += ", data: \""+screenString(lx->data)+"\"";
    result += " }";
    return result;
}

QString Module::dump() const
{
    QString result;
    result = "{\n";
    result += "\t\"header\": {\n";
    result += "\t\t\"name\": \""+header.name+"\",\n";
    if (!header.uses.isEmpty()) {
        result += "\t\t\"uses\": [";
        for (int i=0; i<header.uses.size(); i++) {
            if (i>0)
                result += ", ";
            result += "\""+header.uses.toList()[i]+"\"";
        }
        result += "],\n"; // end uses
    }
    result += "\t\t\"algorhitms\": [\n";
    for (int i=0; i<header.algorhitms.size(); i++) {
        result += "\t\t\t\""+header.algorhitms[i]->header.name+"\"";
        if (i<header.algorhitms.size()-1)
            result += ",";
        result += "\n";
    }
    result += "\t\t]\n"; // end public algorhitms

    result += "\t},\n"; // end header

    result += "\t\"implementation\": {\n";
//    if (!impl.beginLexems.isEmpty()) {
//        result += "\t\"beginLexems\": [\n";
//        for (int i=0; i<impl.beginLexems.size(); i++) {
//            result += "\t\t"+dumpLexem(impl.beginLexems.at(i));
//            if (i<impl.beginLexems.size()-1)
//                result += ",";
//            result += "\n";
//        }
//        result += "\t],\n";
//    }
    if (!impl.globals.isEmpty()) {
    result += "\t\t\"globals\": [\n";
        for (int i=0; i<impl.globals.size(); i++) {
            result += addIndent(impl.globals[i]->dump(), 3);
            if (i<impl.globals.size()-1) {
                result += ",";
            }
            result += "\n";
        }
        result += "\t\t],\n"; // end globals
    }
    if (!impl.initializerBody.isEmpty()) {
        result += "\t\t\"initializer\": [\n";
        for (int i=0; i<impl.initializerBody.size(); i++) {
            result += addIndent(impl.initializerBody[i]->dump(), 3);
            if (i<impl.initializerBody.size()-1) {
                result += ",";
            }
            result += "\n";
        }
        result += "\t\t],\n"; // end initializer
    }
    result += "\t\t\"algorhitms\": [\n";
    for (int i=0; i<impl.algorhitms.size(); i++) {
        result += addIndent(impl.algorhitms[i]->dump(), 3);
        if (i<impl.algorhitms.size()-1) {
            result += ",";
        }
        result += "\n";
    }
    result += "\t\t]"; // end algorhitms implementation
//    if (!impl.endLexems.isEmpty()) {
//        result += ",\n";
//        result += "\t\"endLexems\": [\n";
//        for (int i=0; i<impl.endLexems.size(); i++) {
//            result += "\t\t"+dumpLexem(impl.endLexems.at(i));
//            if (i<impl.endLexems.size()-1)
//                result += ",";
//            result += "\n";
//        }
//        result += "\t]\n";
//    }
//    else {
//        result += "\n";
//    }
    result += "\n";
    result += "\t}\n";
    result += "} /* end module '"+header.name+"' */";
    return result;
}

}
