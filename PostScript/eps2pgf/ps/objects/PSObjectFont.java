/*
 * This file is part of Eps2pgf.
 *
 * Copyright 2007-2009 Paul Wagenaars
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package net.sf.eps2pgf.ps.objects;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.List;
import java.util.Properties;
import java.util.logging.Logger;

import org.fontbox.afm.AFMParser;
import org.fontbox.afm.CharMetric;
import org.fontbox.afm.FontMetric;
import org.fontbox.util.BoundingBox;

import net.sf.eps2pgf.ProgramError;
import net.sf.eps2pgf.ps.Interpreter;
import net.sf.eps2pgf.ps.Matrix;
import net.sf.eps2pgf.ps.errors.PSError;
import net.sf.eps2pgf.ps.errors.PSErrorInvalidFont;
import net.sf.eps2pgf.ps.errors.PSErrorRangeCheck;
import net.sf.eps2pgf.ps.errors.PSErrorTypeCheck;
import net.sf.eps2pgf.ps.errors.PSErrorUnregistered;
import net.sf.eps2pgf.ps.errors.PSErrorVMError;
import net.sf.eps2pgf.ps.resources.encodings.ISOLatin1Encoding;
import net.sf.eps2pgf.ps.resources.encodings.StandardEncoding;
import net.sf.eps2pgf.ps.resources.encodings.SymbolEncoding;
import net.sf.eps2pgf.ps.resources.fonts.FontManager;
import net.sf.eps2pgf.ps.resources.fonts.PSObjectFontMetrics;

/**
 * Wrapper around a font dictionary. This class provides methods to handle the
 * font dictionary.
 * 
 * @author Paul Wagenaars
 */
public class PSObjectFont extends PSObjectDict implements Cloneable {
    
    /** The next font id. */
    private static int nextFID = 0;
    
    /** The log. */
    private static final Logger LOG =
                                    Logger.getLogger("net.sourceforge.eps2pgf");

    //
    // Standard fields in font dictionary
    //
    /** The fontinfo field name. */
    public static final PSObjectName KEY_FONTINFO =
        new PSObjectName("/FontInfo");
    
    /** The fontname field name. */
    public static final PSObjectName KEY_FONTNAME =
        new PSObjectName("/FontName");
    
    /** The encoding field name. */
    public static final PSObjectName KEY_ENCODING =
        new PSObjectName("/Encoding");
    
    /** The painttype field name. */
    public static final PSObjectName KEY_PAINTTYPE =
        new PSObjectName("/PaintType");
    
    /** The fonttype field name. */
    public static final PSObjectName KEY_FONTTYPE =
        new PSObjectName("/FontType");
    
    /** The fontmatrix field name. */
    public static final PSObjectName KEY_FONTMATRIX =
        new PSObjectName("/FontMatrix");
    
    /** The fontbbox field name. */
    public static final PSObjectName KEY_FONTBBOX =
        new PSObjectName("/FontBBox");
    
    /** The uniqueid field name. */
    public static final PSObjectName KEY_UNIQUEID =
        new PSObjectName("/UniqueID");
    
    /** The metrics field name. */
    public static final PSObjectName KEY_METRICS =
        new PSObjectName("/Metrics");
    
    /** The strokewidth field name. */
    public static final PSObjectName KEY_STROKEWIDTH =
        new PSObjectName("/StrokeWidth");
    
    /** The private field name. */
    public static final PSObjectName KEY_PRIVATE =
        new PSObjectName("/Private");
    
    /** The charstrings field name. */
    public static final PSObjectName KEY_CHARSTRINGS =
        new PSObjectName("/CharStrings");
    
    /** The fid field name. */
    public static final PSObjectName KEY_FID =
        new PSObjectName("/FID");
    
    //
    // Standard fields in Private Dictionary
    //
    /** The subrs field name. */
    public static final PSObjectName KEY_PRV_SUBRS =
        new PSObjectName("/Subrs");
    
    //
    // Eps2pgf specific fields in font dictionary
    //
    /** The latexprecode field name. */
    public static final PSObjectName KEY_LATEXPRECODE =
        new PSObjectName("/LatexPreCode");
    
    /** The latexpostcode field name. */
    public static final PSObjectName KEY_LATEXPOSTCODE =
        new PSObjectName("/LatexPostCode");
    
    /** The afm field name. */
    public static final PSObjectName KEY_AFM =
        new PSObjectName("/AFM");
    
    /** The texstrings field name. */
    public static final PSObjectName KEY_TEXSTRINGS =
        new PSObjectName("/TexStrings");
    
    //
    // Field specific for Type 3 fonts
    //
    /** The BuildGlyph field name. */
    public static final PSObjectName KEY_BUILDGLYPH =
        new PSObjectName("/BuildGlyph");
    
    /** The BuildChar field name. */
    public static final PSObjectName KEY_BUILDCHAR =
        new PSObjectName("/BuildChar");
    
    
    //
    // Some parameters are not stored in the dictionary for efficiency reasons
    // and to prevent problems with save/restore.
    //
    /** FontMatrix variable. */
    private Matrix fontMatrix;
    

    /**
     * Create a new empty instance. It sets the FID, font type and default
     * matrix.
     * 
     * @param interpreter The interpreter.
     * 
     * @throws PSErrorVMError Virtual memory error.
     * @throws ProgramError This shouldn't happen, it indicates a bug.
     */
    public PSObjectFont(final Interpreter interpreter)
            throws PSErrorVMError, ProgramError {
        
        super(interpreter);
        
        setFID();
        setKey(KEY_FONTTYPE, new PSObjectInt(1));
        setKey(KEY_FONTMATRIX,
                (new Matrix(0.001, 0, 0, 0.001, 0, 0)).toArray(getInterp()));
    }
    
    /**
     * Creates a new instance of PSObjectFont by loading it from disk.
     * 
     * @param resourceDir Resource directory with font information
     * @param fontName Name of the font to load
     * @param interpreter The interpreter
     * 
     * @throws PSErrorInvalidFont the PS error invalid font
     * @throws ProgramError This shouldn't happen, it indicates a bug.
     * @throws PSErrorVMError Virtual memory error.
     */
    public PSObjectFont(final File resourceDir, final String fontName,
            final Interpreter interpreter)
            throws PSErrorVMError, PSErrorInvalidFont, ProgramError {
        
        super(interpreter);

        File fontFile = new File(resourceDir, FontManager.FONTDESC_DIR_NAME
                + File.separator + fontName + ".font");

        Properties props;
        try {
            FileInputStream in = new FileInputStream(fontFile);
            props = new Properties();
            props.loadFromXML(in);
            in.close();
        } catch (FileNotFoundException e) {
            throw new PSErrorInvalidFont();
        } catch (IOException e) {
            throw new PSErrorInvalidFont();
        }
        
        // Setting the dictionary keys with font info
        setKey(KEY_FONTTYPE, new PSObjectInt(1));
        setKey(KEY_FONTMATRIX,
                (new Matrix(0.001, 0, 0, 0.001, 0, 0)).toArray(interpreter));
        setKey(KEY_FONTNAME, new PSObjectName(fontName, true));
        setFID();
        String encoding = props.getProperty("encoding", "Standard");
        if (encoding.equals("Standard")) {
            setKey(KEY_ENCODING,
                    new PSObjectArray(StandardEncoding.get(), interpreter));
        } else if (encoding.equals("ISOLatin1")) {
            setKey(KEY_ENCODING,
                    new PSObjectArray(ISOLatin1Encoding.get(), interpreter));
        } else if (encoding.equals("Symbol")) {
            setKey(KEY_ENCODING, new PSObjectArray(SymbolEncoding.get(),
                    interpreter));
        } else {
            LOG.severe("Unknown encoding: " + encoding);
            throw new PSErrorInvalidFont();
        }
        setKey(KEY_PAINTTYPE, new PSObjectInt(2));
        
        setKey(KEY_LATEXPRECODE,
                new PSObjectString(props.getProperty("latexprecode", ""),
                        getInterp()));
        setKey(KEY_LATEXPOSTCODE,
                new PSObjectString(props.getProperty("latexpostcode", ""),
                        getInterp()));
        
        String texStringName = props.getProperty("texstrings", "default");
        setKey(KEY_TEXSTRINGS,
                FontManager.getTexStringDict(texStringName, interpreter));
        
        FontMetric fontMetrics = loadAfm(resourceDir, fontName);
        setKey(KEY_AFM, new PSObjectFontMetrics(fontMetrics));
        
        // An AFM file does not specify CharStrings. Instead, we make a fake
        // entry.
        List< ? > charMetrics = fontMetrics.getCharMetrics();
        PSObjectDict charStrings =
            new PSObjectDict(charMetrics.size(), interpreter);
        for (Object obj : charMetrics) {
            if (obj instanceof CharMetric) {
                CharMetric cm = (CharMetric) obj;
                charStrings.setKey(cm.getName(), "");
            }
        }
        setKey(KEY_CHARSTRINGS, charStrings);
    }
    
    /**
     * Creates a new font dictionary with dict as dictionary.
     * 
     * @param dict Dictionary to use as font dictionary
     */
    public PSObjectFont(final PSObjectDict dict) {
        super(dict);
    }
    
    /**
     * Assert that this font contains all fields that are expected from a font.
     * If they don't exist they will be created.
     * 
     * @return Returns <code>true</code> if this font was already valid. Returns
     * <code>false</code> when one or more fields were missing.
     * 
     * @throws PSError A PostScript error occurred.
     * @throws ProgramError This shouldn't happen, it indicates a bug.
     */
    public boolean assertValidFont() throws PSError, ProgramError {
        boolean alreadyValid = true;
        
        String fontname = getFontName();

        // Copy the "special" variables from the dictionary to their own
        // variables.
        if (known(KEY_FONTMATRIX)) {
            fontMatrix = lookup(KEY_FONTMATRIX).toArray().toMatrix();
        }
        
        // See if texstrings are defined for this font. If not, copy standard
        // texstrings.
        if (!known(KEY_TEXSTRINGS)) {
            setKey(KEY_TEXSTRINGS,
                 FontManager.getTexStringDictByFontname(fontname, getInterp()));
            alreadyValid = false;
        }
        
        // If the LaTeX pre- and post codes are unknown, try to derive
        // them from the font name.
        if (!known(KEY_LATEXPRECODE) || !known(KEY_LATEXPOSTCODE)) {
            String[][] fontTypes = {
                    {"Serif",     "\\textrm{", "}"},
                    {"Roman",     "\\textrm{", "}"},
                    {"Sans",      "\\textsf{", "}"},
                    {"Mono",      "\\texttt{", "}"},
                    {"Monospace", "\\texttt{", "}"},
                    {"Bold",      "\\textbf{", "}"},
                    {"Oblique",   "\\textsl{", "}"},
                    {"Obli",      "\\textsl{", "}"},
                    {"Slanted",   "\\textsl{", "}"},
                    {"Italic",    "\\textit{", "}"},
                    {"Ital",      "\\textit{", "}"},
                    {"CMMI",      "\\textit{", "}"}};
            String pre = "";
            String post = "";
            // Append an "X" to the font name to make sure that
            // there is at least one [^a-z] character after the
            // font type string.
            String augmentedFontname = fontname + "X"; 
            for (int i = 0; i < fontTypes.length; i++) {
                String regexp = ".*" + fontTypes[i][0] + "[^a-z].*";
                if (augmentedFontname.matches(regexp)) {
                    pre += fontTypes[i][1];
                    post = fontTypes[i][2] + post;
                }
            }
            setKey(KEY_LATEXPRECODE, new PSObjectString(pre, getInterp()));
            setKey(KEY_LATEXPOSTCODE, new PSObjectString(post, getInterp()));
            alreadyValid = false;
        }
        
        // Check for font metrics
        if (!known(KEY_AFM)) {
            // Apparently there are no metrics. Try to extract it from the 
            // character descriptions.
            alreadyValid = false;
            setKey(KEY_AFM, new PSObjectFontMetrics(this, getInterp()));
            LOG.fine("Creating font metrics for font " + getFontName());
        }
        
        return alreadyValid;
    }
    
    /**
     * Convert an array with character names to a string to corresponding
     * LaTeX code.
     * 
     * @param charNames Character names to convert
     * 
     * @return LaTeX code corresponding to supplied character names
     * 
     * @throws PSError a PostScript error occurred
     * @throws ProgramError This shouldn't happen, it indicates a bug.
     */
    public String charNames2texStrings(final PSObjectArray charNames)
            throws PSError, ProgramError {
        
        assertValidFont();
        
        StringBuilder str = new StringBuilder();
        PSObjectDict texStrings = lookup(KEY_TEXSTRINGS).toDict();
        PSObjectString preCode = lookup(KEY_LATEXPRECODE).toPSString();
        PSObjectString postCode = lookup(KEY_LATEXPOSTCODE).toPSString();
        
        str.append(preCode.toString());
        for (int i = 0; i < charNames.size(); i++) {
            try {
                PSObjectName charName = charNames.get(i).toName();
                PSObject code = texStrings.lookup(charName);
                if (code == null) {
                    throw new PSErrorUnregistered("TexString for "
                            + charNames.get(i).isis() + " is unknown.");
                }
                str.append(code.toPSString().toString());
            } catch (PSErrorRangeCheck e) {
                // This can never happen inside this for loop
            }
        }
        str.append(postCode.toString());

        return str.toString();
    }
    
    /**
     * Create a deep copy of this font object.
     * 
     * @return The copy of this object.
     */
    @Override
    public PSObjectFont clone() {
        PSObjectFont copy;
        copy = (PSObjectFont) super.clone();
        
        if (fontMatrix != null) {
            copy.fontMatrix = fontMatrix.clone();
        }
        
        return copy;
    }
    
    /**
     * Converts a single character defined as character name or CID to a string
     * with a single character.
     * 
     * @param nameOrCid The name or CID of the character.
     * @param interp The interpreter.
     * 
     * @return The string with the character.
     * 
     * @throws PSError A PostScript error occurred.
     */
    public PSObjectString encodeChar(final PSObject nameOrCid,
            final Interpreter interp) throws PSError {
        
        PSObjectArray enc = get(KEY_ENCODING).toArray();
        
        String string = "";
        for (int i = 0; i < enc.size(); i++) {
            if (nameOrCid.eq(enc.get(i))) {
                string += (char) i;
                break;
            }
        }
        
        return new PSObjectString(string, interp);
    }
    
    /**
     * Indicates whether some other object is equal to this one.
     * Required when used as index in PSObjectDict
     * 
     * @param obj The object to compare to.
     * 
     * @return True, if equal.
     */
    @Override
    public boolean equals(final Object obj) {
        return (this == obj);
    }
    
    /**
     * Returns a hash code value for the object.
     * 
     * @return Hash code of this object.
     */
    @Override
    public int hashCode() {
        return getMap().hashCode();
    }
    
    /**
     * Get the bounding box of a text (defined by a series of charStrings.
     * 
     * @param charNames Character names of the text for which the bounding box
     * must be determined.
     * 
     * @return Bounding box
     * 
     * @throws PSError a PostScript error occurred
     * @throws ProgramError This shouldn't happen, it indicates a bug.
     */
    public BoundingBox getBBox(final PSObjectArray charNames)
            throws PSError, ProgramError {
        //charNames = replaceLigatures(charNames);
        
        BoundingBox bbox = new BoundingBox();

        // Determine upper and lower boundary of bounding box
        double minY = Double.POSITIVE_INFINITY;
        double maxY = Double.NEGATIVE_INFINITY;
        CharMetric cmFirstChar = new CharMetric();
        CharMetric cmLastChar = new CharMetric();
        for (int i = 0; i < charNames.size(); i++) {
            try {
                String charName = charNames.get(i).toName().toString();
                CharMetric cm = getCharMetric(charName);
                BoundingBox charBbox = cm.getBoundingBox();
                minY = Math.min(minY, charBbox.getLowerLeftY());
                maxY = Math.max(maxY, charBbox.getUpperRightY());
                
                // Save 1st and last character bbox. These are used below
                // the determine left and right boundary.
                if (i == 0) {
                    cmFirstChar = cm;
                }
                if (i == (charNames.size() - 1)) {
                    cmLastChar = cm;
                }
            } catch (PSErrorRangeCheck e) {
                    // This can never happen inside the for loop
            }
        }
        
        // Determine left and right boundary of bounding box
        double scaling = getFontMatrix().getMeanScaling();
        double leftX = cmFirstChar.getBoundingBox().getLowerLeftX() * scaling;
        double rightX = getWidth(charNames) - cmLastChar.getWx() * scaling
                + cmLastChar.getBoundingBox().getUpperRightX() * scaling;

        bbox.setLowerLeftX((float) leftX);
        bbox.setLowerLeftY((float) (minY * scaling));
        bbox.setUpperRightX((float) rightX);
        bbox.setUpperRightY((float) (maxY * scaling));
        return  bbox;
    }
    
    /**
     * Retrieves the metrics for a single character.
     * 
     * @param charName the char name
     * 
     * @return the char metric
     * 
     * @throws PSError A PostScript error occurred.
     * @throws ProgramError This shouldn't happen, it indicates a bug.
     */
    CharMetric getCharMetric(final String charName)
            throws PSError, ProgramError {
        FontMetric fm = getFontMetric();
        List< ? > charMetrics = fm.getCharMetrics();
        for (Object obj : charMetrics) {
            if (!(obj instanceof CharMetric)) {
                throw new PSErrorTypeCheck();
            }
            CharMetric cm = (CharMetric) obj;
            if (charName.equals(cm.getName())) {
                return cm;
            }
        }
        
        // CharMetric was not found, let's return an empty one
        CharMetric empty = new CharMetric();
        BoundingBox bbox = new BoundingBox();
        bbox.setLowerLeftX(0);
        bbox.setLowerLeftY(0);
        bbox.setUpperRightX(0);
        bbox.setUpperRightY(0);
        empty.setBoundingBox(bbox);
        empty.setWx(0);
        return empty;
    }
    
    /**
     * Return the encoding in this font.
     * 
     * @return Encoding defined by this font
     * 
     * @throws PSErrorTypeCheck the PS error type check
     */
    public PSObjectArray getEncoding() throws PSErrorTypeCheck {
        return lookup(KEY_ENCODING).toArray();
    }
    
    /**
     * Returns the font ID, if defined. Returns -1 when no FID is defined.
     * 
     * @return the FID
     */
    int getFID() {
        try {
            return lookup(KEY_FID).toInt();
        } catch (PSErrorTypeCheck e) {
            // FID is not an integer
            return -1;
        } catch (NullPointerException e) {
            // No FID is defined
            return -1;
        }
    }
    
    /**
     * Return the FontMatrix.
     * 
     * @return FontMatrix defined by this font
     */
    public Matrix getFontMatrix() {
        return fontMatrix;
    }
    
    /**
     * Returns the font metrics of this font.
     * 
     * @return Font metrics
     * 
     * @throws PSError A PostScript error occurred.
     * @throws ProgramError This shouldn't happen, it indicates a bug.
     */
    public FontMetric getFontMetric() throws PSError, ProgramError {
        assertValidFont();
        return lookup(KEY_AFM).toFontMetric();
    }
    
    /**
     * Returns the fontname of this font.
     * 
     * @return Fontname
     */
    String getFontName() {
        if (known(KEY_FONTNAME)) {
            PSObject fontNameObj = lookup(KEY_FONTNAME);
            return fontNameObj.toString();
        }
        
        // No font name is defined in this dictionary. Instead we use the key
        // in the FontDirectory that is associated with this font. This key is
        // stored in the font itself behind the FontManager.FONT_DICT_KEY key.
        if (known(FontManager.FONT_DICT_KEY)) {
            PSObject keyObj = lookup(FontManager.FONT_DICT_KEY);
            return keyObj.toString();
        }
        
        return "";
    }
    
    /**
     * Returns the font size in pt (= 1/72 inch).
     * 
     * @return Font size of this font in pt (= 1/72 inch)
     * 
     * @throws PSErrorTypeCheck the PS error type check
     * @throws PSErrorRangeCheck the PS error range check
     */
    public double getFontSize() throws PSErrorTypeCheck, PSErrorRangeCheck {
        return fontMatrix.getMeanScaling() * 1000.0;
    }
    
    /**
     * Determines the total width of a string.
     * 
     * @param charNames Array with character names describing the string
     * 
     * @return Width of the string in pt (= 1/72 inch)
     * 
     * @throws PSError the PS error
     * @throws ProgramError This shouldn't happen, it indicates a bug.
     */
    public double getWidth(final PSObjectArray charNames)
            throws PSError, ProgramError {
        //charNames = replaceLigatures(charNames);
        double width = 0;
        for (int i = 0; i < charNames.size(); i++) {
            try {
                String charName = charNames.get(i).toName().toString();
                CharMetric cm = getCharMetric(charName);
                double scaling = getFontMatrix().getMeanScaling();
                width += cm.getWx() * scaling;
            } catch (PSErrorRangeCheck e) {
                    // This can never happen inside the for loop
            }
        }
        return width;
    }
    
    /**
     * Creates a human-readable string representation of this font.
     * 
     * @return Human-readable string representation of this font. See the
     * PostScript specification on the == operator for more info.
     */
    @Override
    public String isis() {
        return "-font " + getFontName() + " (FID " + getFID() + ")-";
    }

    /**
     * Load font metrics (*.afm) from the resource directory.
     * 
     * @param resourceDir Resource directory with font information
     * @param fontName Name of the font to load
     * 
     * @return Font metrics of requested font
     * 
     * @throws PSErrorInvalidFont The PS error "invalidfont".
     */
    public FontMetric loadAfm(final File resourceDir, final String fontName)
            throws PSErrorInvalidFont {
        
        File afmFile = new File(resourceDir, FontManager.AFM_DIR_NAME
                + File.separator + fontName + ".afm");
        
        FontMetric fontMetric;
        try {
            FileInputStream in = new FileInputStream(afmFile);
            AFMParser afm = new AFMParser(in);
            afm.parse();
            fontMetric = afm.getResult();
            in.close();
        } catch (FileNotFoundException e) {
            throw new PSErrorInvalidFont();
        } catch (IOException e) {
            throw new PSErrorInvalidFont();
        }
        
        return fontMetric;
    }
    
    /**
     * Looks up a key in this dictionary.
     * 
     * @return Object associated with the key, <code>null</code> if no object is
     * associated with this key.
     * 
     * @param key Key of the entry to look up.
     */
    @Override
    public PSObject lookup(final PSObject key) {
        // If this is one of the special keys we also store it in a local
        // variable, return that variable.
        try {
            if (KEY_FONTMATRIX.eq(key)) {
                return fontMatrix.toArray(getInterp());
            }
        } catch (PSErrorVMError e) {
            /* empty block */
        } catch (NullPointerException e) {
            /* empty block */
        }
        
        return super.lookup(key);
    }
    
    /**
     * Check if this font already has a font ID (FID). If not, set the font ID
     * for this font to the next available ID.
     * 
     * @return New font ID
     */
    public int setFID() {
        int fid = getFID();
        if (fid < 0) {
            fid = nextFID++;
            setKey(KEY_FID, new PSObjectInt(fid));
        }
        return fid;
    }
    
    /**
     * Sets a key in the dictionary.
     * 
     * @param key Key of the new dictionary entry.
     * @param value Value of the new dictionary entry.
     */
    @Override
    public void setKey(final PSObject key, final PSObject value) {
        // If this is one of the special keys we also store it in a local
        // variable.
        if (KEY_FONTMATRIX.eq(key)) {
            try {
                fontMatrix = value.toArray().toMatrix();
            } catch (PSError e) {
                fontMatrix = null;
            }
        }
        
        super.setKey(key, value);
    }
    
    /**
     * Return this object as regular dictionary.
     * 
     * @return This object
     */
    @Override
    public PSObjectDict toDict() {
        return this;
    }

    /**
     * Return this object as font dictionary.
     * 
     * @return This object
     */
    @Override
    public PSObjectFont toFont() {
        return this;
    }

}
