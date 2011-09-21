/****************************************************************************
** BBB_Form meta object code from reading C++ file 'bbb_Form.h'
**
** Created: Sat Jan 31 12:28:23 2009
**      by: The Qt MOC ($Id: moc_yacc.cpp 2051 2007-02-21 10:04:20Z chehrlic $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "bbb_Form.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *BBB_Form::className() const
{
    return "BBB_Form";
}

QMetaObject *BBB_Form::metaObj = 0;
static QMetaObjectCleanUp cleanUp_BBB_Form( "BBB_Form", &BBB_Form::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString BBB_Form::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "BBB_Form", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString BBB_Form::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "BBB_Form", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* BBB_Form::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"BBB_Form", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_BBB_Form.setMetaObject( metaObj );
    return metaObj;
}

void* BBB_Form::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "BBB_Form" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool BBB_Form::qt_invoke( int _id, QUObject* _o )
{
    return QWidget::qt_invoke(_id,_o);
}

bool BBB_Form::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool BBB_Form::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool BBB_Form::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
