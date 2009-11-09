package edu.cmu.sphinx.demo.raw;

import edu.cmu.sphinx.demo.raw.CommonConfiguration;
import edu.cmu.sphinx.frontend.util.AudioFileDataSource;
import edu.cmu.sphinx.jsapi.JSGFGrammar;
import edu.cmu.sphinx.linguist.acoustic.UnitManager;
import edu.cmu.sphinx.linguist.acoustic.tiedstate.Sphinx3Loader;
import edu.cmu.sphinx.linguist.acoustic.tiedstate.TiedStateAcousticModel;
import edu.cmu.sphinx.linguist.dictionary.FastDictionary;
import edu.cmu.sphinx.linguist.flat.FlatLinguist;
import edu.cmu.sphinx.util.LogMath;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;

/**
 * User: peter
 * Date: Nov 2, 2009
 * Time: 4:52:55 PM
 * <p/>
 * Copyright 1999-2004 Carnegie Mellon University.
 * Portions Copyright 2004 Sun Microsystems, Inc.
 * Portions Copyright 2004 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * <p/>
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 */
class TranscriberConfiguration extends CommonConfiguration {

    public TranscriberConfiguration(String root) throws MalformedURLException {
        super(root);
    }

    protected void initCommon() {
        super.initCommon();

        this.absoluteBeamWidth = -1;
        this.relativeBeamWidth = 1E-80;
        this.wordInsertionProbability = 1E-36;
        this.languageWeight = 8.0f;
    }

    protected void initModels() throws MalformedURLException {

        this.unitManager = new UnitManager();

        this.modelLoader = new Sphinx3Loader(
                "file:"+root+"/models/acoustic/tidigits/model.props",
                logMath,
                unitManager,
                true,
                true,
                39,
                "file:"+root+"/models/acoustic/tidigits/wd_dependent_phone.500.mdef",
                "file:"+root+"/models/acoustic/tidigits/wd_dependent_phone.cd_continuous_8gau/",
                0.0f,
                1e-7f,
                0.0001f,
                true);

        this.model = new TiedStateAcousticModel(modelLoader, unitManager, true);

        this.dictionary = new FastDictionary(
                new URL("file:"+root+"/models/acoustic/tidigits/dictionary"),
                new URL("file:"+root+"/models/acoustic/tidigits/fillerdict"),
                new ArrayList<URL>(),
                false,
                "<sil>",
                false,
                false,
                unitManager);
    }

    protected void initLinguist() throws MalformedURLException {

        this.grammar = new JSGFGrammar(
                // URL baseURL,
                new URL("file:"+root+"/src/apps/edu/cmu/sphinx/demo/transcriber/"),
                logMath, // LogMath logMath,
                "digits", // String grammarName,
                false, // boolean showGrammar,
                false, // boolean optimizeGrammar,
                false, // boolean addSilenceWords,
                false, // boolean addFillerWords,
                dictionary // Dictionary dictionary
        );

        this.linguist = new FlatLinguist(
                model, // AcousticModel acousticModel,
                logMath, // LogMath logMath,
                grammar, // Grammar grammar,
                unitManager, // UnitManager unitManager,
                wordInsertionProbability, // double wordInsertionProbability,
                1.0, // double silenceInsertionProbability,
                1.0, // double fillerInsertionProbability,
                1.0, // double unitInsertionProbability,
                languageWeight, // float languageWeight,
                false, // boolean dumpGStates,
                false, // boolean showCompilationProgress,
                false, // boolean spreadWordProbabilitiesAcrossPronunciations,
                false, // boolean addOutOfGrammarBranch,
                1.0, // double outOfGrammarBranchProbability,
                1.0, // double phoneInsertionProbability,
                null // AcousticModel phoneLoopAcousticModel
        );
    }

}