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

package net.sf.eps2pgf.ps;

/**
 * Path section representing a PostScript moveto operator.
 * @author Paul Wagenaars
 */
public class Moveto extends PathSection implements Cloneable {
    
    /**
     * Create a new Movoto instance.
     */
    public Moveto() {
        int nr = nrParams();
        for (int i = 0; i < nr; i++) {
            setParam(i, Double.NaN);
        }        
    }
    
    /**
     * Create a new Moveto instance.
     * 
     * @param x X-coordinate (in device coordinates, micrometer)
     * @param y Y-coordinate (in device coordinates, micrometer)
     */
    public Moveto(final double x, final double y) {
        setParam(0, x);
        setParam(1, y);
        int nr = nrParams();
        for (int i = 2; i < nr; i++) {
            setParam(i, Double.NaN);
        }
    }
}
