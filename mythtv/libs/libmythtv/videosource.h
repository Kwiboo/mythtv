#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include "libmyth/settings.h"
#include "libmyth/mythcontext.h"
#include <qlistview.h>
#include <qdialog.h>
#include <vector>

class VideoSource;
class VSSetting: public SimpleDBStorage {
protected:
    VSSetting(const VideoSource& _parent, QString name):
        SimpleDBStorage("videosource", name),
        parent(_parent) {
        setName(name);
    };

    virtual QString setClause(void);
    virtual QString whereClause(void);

    const VideoSource& parent;
};

class VideoSource: public VerticalConfigurationGroup, public ConfigurationDialog {
public:
    VideoSource(MythContext* context): ConfigurationDialog(context) {
        // must be first
        addChild(id = new ID());
        addChild(new Name(*this));
        addChild(new XMLTVGrab(*this));
    };
        
    int getSourceID(void) const { return id->intValue(); };

    void loadByID(QSqlDatabase* db, int id);

    static void fillSelections(QSqlDatabase* db, StringSelectSetting* setting);
    static QString idToName(QSqlDatabase* db, int id);

private:
    class ID: virtual public IntegerSetting,
              public AutoIncrementStorage {
    public:
        ID():
            AutoIncrementStorage("videosource", "sourceid") {
            setName("VideoSourceName");
            setVisible(false);
        };
        virtual QWidget* configWidget(QWidget* parent, const char* widgetName = 0) {
            (void)parent; (void)widgetName;
            return NULL;
        };
    };
    class Name: virtual public VSSetting,
                virtual public LineEditSetting {
    public:
        Name(const VideoSource& parent):
            VSSetting(parent, "name") {
            setLabel("Video source name");
        };
    };

    class XMLTVGrab: public ComboBoxSetting, public VSSetting {
    public:
        XMLTVGrab(const VideoSource& parent): VSSetting(parent, "xmltvgrabber") {
            setLabel("XMLTV listings grabber");
            addSelection("tv_grab_na");
            addSelection("tv_grab_de");
            addSelection("tv_grab_sn");
            addSelection("tv_grab_uk");
            addSelection("tv_grab_uk_rt");
        };
    };

private:
    ID* id;
};

class CaptureCard;
class CCSetting: virtual public Setting, public SimpleDBStorage {
protected:
    CCSetting(const CaptureCard& _parent, QString name):
        SimpleDBStorage("capturecard", name),
        parent(_parent) {
        setName(name);
    };

    int getCardID(void) const;

protected:
    virtual QString setClause(void);
    virtual QString whereClause(void);
private:
    const CaptureCard& parent;
};

class VideoDevice: public PathSetting, public CCSetting {
public:
    VideoDevice(const CaptureCard& parent):
        PathSetting(true),
        CCSetting(parent, "videodevice") {
        setLabel("Video device");
        addSelection("/dev/video");
        addSelection("/dev/video0");
        addSelection("/dev/video1");
        addSelection("/dev/v4l/video0");
        addSelection("/dev/v4l/video1");
    };
};

class AudioDevice: public PathSetting, public CCSetting {
public:
    AudioDevice(const CaptureCard& parent):
        PathSetting(true),
        CCSetting(parent, "audiodevice") {
        setLabel("Audio device");
        addSelection("/dev/dsp");
        addSelection("/dev/dsp1");
        addSelection("/dev/sound/dsp");
    };
};

class CardType: public ComboBoxSetting, public CCSetting {
public:
    CardType(const CaptureCard& parent):
        CCSetting(parent, "cardtype") {
        setLabel("Card type");
        addSelection("V4L");
    };
};

class AudioRateLimit: public ComboBoxSetting, public CCSetting {
public:
    AudioRateLimit(const CaptureCard& parent):
        CCSetting(parent, "audioratelimit") {
        setLabel("Audio sampling rate limit");
        addSelection("0");
        addSelection("32000");
        addSelection("44100");
        addSelection("48000");
    };
};

class CaptureCard: public VerticalConfigurationGroup, public ConfigurationDialog {
public:
    CaptureCard(MythContext* context): ConfigurationDialog(context) {
        setLabel("Capture card");
        // must be first
        addChild(id = new ID());
        addChild(new VideoDevice(*this));
        addChild(new AudioDevice(*this));
        addChild(new AudioRateLimit(*this));
    };

    int getCardID(void) const {
        return id->intValue();
    };

    void loadByID(QSqlDatabase* db, int id);

    static void fillSelections(QSqlDatabase* db, StringSelectSetting* setting);

private:
    class ID: virtual public IntegerSetting,
              public AutoIncrementStorage {
    public:
        ID():
            AutoIncrementStorage("capturecard", "cardid") {
            setVisible(false);
            setName("ID");
        };
        virtual QWidget* configWidget(QWidget* parent, const char* widgetName = 0) {
            (void)parent; (void)widgetName;
            return NULL;
        };
    };


private:
    ID* id;
};

class CardInput;
class CISetting: virtual public Setting, public SimpleDBStorage {
protected:
    CISetting(const CardInput& _parent, QString name):
        SimpleDBStorage("cardinput", name),
        parent(_parent) {
        setName(name);
    };

    int getInputID(void) const;

    void fillSelections(QSqlDatabase* db);

protected:
    virtual QString setClause(void);
    virtual QString whereClause(void);
private:
    const CardInput& parent;
};

class CardID: public SelectLabelSetting, public CISetting {
public:
    CardID(const CardInput& parent):
        CISetting(parent, "cardid") {
        setLabel("Capture device");
    };

    virtual void load(QSqlDatabase* db) {
        fillSelections(db);
        CISetting::load(db);
    };

    void fillSelections(QSqlDatabase* db) {
        CaptureCard::fillSelections(db, this);
    };
};


class SourceID: public ComboBoxSetting, public CISetting {
public:
    SourceID(const CardInput& parent):
        CISetting(parent, "sourceid") {
        setLabel("Video source");
        addSelection("(None)", "0");
    };

    virtual void load(QSqlDatabase* db) {
        fillSelections(db);
        CISetting::load(db);
    };

    void fillSelections(QSqlDatabase* db) {
        VideoSource::fillSelections(db, this);
    };
};

class InputName: public LabelSetting, public CISetting {
public:
    InputName(const CardInput& parent):
        CISetting(parent, "inputname") {
        setLabel("Input");
    };
};

class CardInput: public VerticalConfigurationGroup, public ConfigurationDialog {
public:
    CardInput(MythContext* context): ConfigurationDialog(context) {
        setLabel("Associate input with source");
        addChild(id = new ID());
        addChild(cardid = new CardID(*this));
        addChild(inputname = new InputName(*this));
        addChild(sourceid = new SourceID(*this));
    };

    int getInputID(void) const { return id->intValue(); };

    void loadByID(QSqlDatabase* db, int id);
    void loadByInput(QSqlDatabase* db, int cardid, QString input);
    QString getSourceName(void) const { return sourceid->getCurrentLabel(); };

    virtual void save(QSqlDatabase* db);

private:
    class ID: virtual public IntegerSetting,
              public AutoIncrementStorage {
    public:
        ID():
            AutoIncrementStorage("cardinput", "cardid") {
            setVisible(false);
            setName("CardInputID");
        };
        virtual QWidget* configWidget(QWidget* parent, const char* widgetName = 0) {
            (void)parent; (void)widgetName;
            return NULL;
        };
    };

private:
    ID* id;
    CardID* cardid;
    InputName* inputname;
    SourceID* sourceid;
    QSqlDatabase* db;
};

class CaptureCardEditor: public ListBoxSetting, public ConfigurationDialog {
    Q_OBJECT
public:
    CaptureCardEditor(MythContext* context, QSqlDatabase* _db):
        ConfigurationDialog(context), db(_db) {
        setLabel("Capture cards");
    };

    virtual int exec(QSqlDatabase* db);
    virtual void load(QSqlDatabase* db);
    virtual void save(QSqlDatabase* db) { (void)db; };

protected slots:
    void edit(int id) {
        CaptureCard cc(m_context);

        if (id != 0)
            cc.loadByID(db,id);

        cc.exec(db);
    };

protected:
    QSqlDatabase* db;
};

class VideoSourceEditor: public ListBoxSetting, public ConfigurationDialog {
    Q_OBJECT
public:
    VideoSourceEditor(MythContext* context, QSqlDatabase* _db):
        ConfigurationDialog(context), db(_db) {
        setLabel("Video sources");
    };

    virtual int exec(QSqlDatabase* db);
    virtual void load(QSqlDatabase* db);
    virtual void save(QSqlDatabase* db) { (void)db; };

protected slots:
    void edit(int id) {
        VideoSource vs(m_context);

        if (id != 0)
            vs.loadByID(db,id);

        vs.exec(db);
    };

protected:
    QSqlDatabase* db;
};

class CardInputEditor: public ListBoxSetting, public ConfigurationDialog {
    Q_OBJECT
public:
    CardInputEditor(MythContext* context, QSqlDatabase* _db):
        ConfigurationDialog(context), db(_db) {
        setLabel("Input connections");
    };

    virtual int exec(QSqlDatabase* db);
    virtual void load(QSqlDatabase* db);
    virtual void save(QSqlDatabase* db) { (void)db; };

protected slots:
    void edit(int id) {
        cardinputs[id]->exec(db);
    };

protected:
    vector<CardInput*> cardinputs;
    QSqlDatabase* db;
};

#endif
