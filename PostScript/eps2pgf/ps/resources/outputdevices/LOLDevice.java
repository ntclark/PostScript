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

package net.sf.eps2pgf.ps.resources.outputdevices;

import java.io.IOException;
import java.io.Writer;

import net.sf.eps2pgf.ps.GraphicsState;
import net.sf.eps2pgf.ps.Image;
import net.sf.eps2pgf.ps.Matrix;
import net.sf.eps2pgf.ps.Path;
import net.sf.eps2pgf.ps.errors.PSErrorIOError;
import net.sf.eps2pgf.ps.objects.PSObjectDict;

/**
 * Device that writes only the labels to the output.
 * 
 * @author Paul Wagenaars
 *
 */
public class LOLDevice implements OutputDevice, Cloneable {
    
    /** Output is written to this writer. */
    private Writer out;
    
    /**
     * Instantiates a new lOL device.
     * 
     * @param pOut Output will be written to this object.
     */
    public LOLDevice(final Writer pOut) {
        out = pOut;
    }

    /**
     * Implements PostScript clip operator.
     * Intersects the area inside the current clipping path with the area
     * inside the current path to produce a new, smaller clipping path.
     * 
     * @param clipPath the clip path
     */
    public void clip(final Path clipPath) {
        
    }

    /**
     * Returns a exact deep copy of this output device.
     * 
     * @return Deep copy of this object.
     */
    @Override
    public LOLDevice clone() {
        LOLDevice copy;
        try {
            copy = (LOLDevice) super.clone();
        } catch (CloneNotSupportedException e) {
            copy = null;
        }
        
        return copy;
    }

    /**
     * Returns a <b>copy</b> default transformation matrix (converts user space
     * coordinates to device space).
     * 
     * @return Default transformation matrix.
     */
    public Matrix defaultCTM() {
        return new Matrix(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    }

    /**
     * Draws a red dot (useful for debugging, don't use otherwise).
     * 
     * @param x X-coordinate of dot.
     * @param y Y-coordinate of dot.
     */
    public void drawDot(final double x, final double y) {

    }

    /**
     * Draws a blue rectangle (useful for debugging, don't use otherwise).
     * 
     * @param lowerLeft Lower-left coordinate.
     * @param upperRight Upper-right coordinate.
     */
    public void drawRect(final double[] lowerLeft,
            final double[] upperRight) {

    }

    /**
     * Ends the current scope.
     */
    public void endScope() {

    }

    /**
     * Set the current clipping path in the graphics state as clipping path in
     * the output document. The even-odd rule is used to determine which point
     * are inside the path.
     * 
     * @param gstate Current graphics state.
     */
    public void eoclip(final GraphicsState gstate) {

    }

    /**
     * Fills a path using the even-odd rule.
     * See the PostScript manual (fill operator) for more info.
     * 
     * @param gstate Current graphics state.
     */
    public void eofill(final GraphicsState gstate) {

    }

    /**
     * Internal Eps2pgf command: eps2pgfgetmetrics
     * It is meant for the cache device. When this command is issued, it will
     * return metrics information about the drawn glyph.
     * 
     * @return Metrics information about glyph.
     */
    public double[] eps2pgfGetMetrics() {
        return null;
    }

    /**
     * Fills a path using the non-zero rule
     * See the PostScript manual (fill operator) for more info.
     * 
     * @param gstate Current graphics state.
     */
    public void fill(final GraphicsState gstate) {

    }

    /**
     * Finalize writing. Normally, this method writes a footer.
     */
    public void finish() {

    }

    /**
     * Initialize before any other methods are called. Normally, this method
     * writes a header.
     */
    public void init() {

    }

    /**
     * Shading fill (shfill PostScript operator).
     * 
     * @param dict Shading to use.
     * @param gstate Current graphics state.
     */
    public void shfill(final PSObjectDict dict, final GraphicsState gstate) {

    }

    /**
     * Draws text.
     * 
     * @param text Exact text to draw
     * @param position Text anchor point in [micrometer, micrometer]
     * @param angle Text angle in degrees
     * @param fontsize in PostScript pt (= 1/72 inch). If fontsize is NaN, the
     * font size is not set and completely determined by LaTeX.
     * @param anchor String with two characters:
     * t - top, c - center, B - baseline b - bottom
     * l - left, c - center, r - right
     * e.g. Br = baseline,right
     * @param gstate Current graphics state.
     * 
     * @throws PSErrorIOError PostScript IO error.
     */
    public void show(final String text, final double[] position,
            final double angle, final double fontsize, final String anchor,
            final GraphicsState gstate) throws PSErrorIOError {
        
        try {
            out.write(String.format("\\overlaylabel(%s,%s)[%s][%s]{%s}%%\n",
                    PGFDevice.COOR_FORMAT.format(position[0]),
                    PGFDevice.COOR_FORMAT.format(position[1]),
                    anchor,
                    PGFDevice.ANGLE_FORMAT.format(angle),
                    text));
        } catch (IOException e) {
            throw new PSErrorIOError();
        }
    }

    /**
     * Starts a new scope.
     */
    public void startScope() {

    }

    /**
     * Implements PostScript stroke operator.
     * 
     * @param gstate Current graphics state.
     */
    public void stroke(final GraphicsState gstate) {

    }

    /**
     * Adds a bitmap image to the output.
     * 
     * @param img The bitmap image to add.
     */
    public void image(final Image img) {
        /* empty block */
    }
}
