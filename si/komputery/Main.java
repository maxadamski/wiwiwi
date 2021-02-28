package com.sample;

import java.util.ArrayList;

import org.kie.api.KieServices;
import org.kie.api.runtime.KieContainer;
import org.kie.api.runtime.KieSession;
import org.kie.api.runtime.rule.QueryResultsRow;
import org.kie.api.runtime.rule.QueryResults;
import org.kie.api.logger.KieRuntimeLogger;

public class Main {
    public static final void main(String[] args) throws Throwable {
		KieServices services = KieServices.Factory.get();
		KieContainer container = services.getKieClasspathContainer();
		KieSession session = container.newKieSession("ksession-rules");
		KieRuntimeLogger logger = services.getLoggers().newFileLogger(session, "test");
		session.fireAllRules();
		logger.close();
    }
}
