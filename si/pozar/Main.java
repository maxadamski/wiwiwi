package com.sample;

import org.kie.api.KieServices;
import org.kie.api.runtime.KieContainer;
import org.kie.api.runtime.KieSession;
import org.kie.api.logger.KieRuntimeLogger;

public class Main {
    public static final void main(String[] args) throws Throwable {
		KieServices services = KieServices.Factory.get();
		KieContainer container = services.getKieClasspathContainer();
		KieSession session = container.newKieSession("ksession-rules");
		KieRuntimeLogger logger = services.getLoggers().newFileLogger(session, "test");

		session.fireAllRules();
		session.insert(new Straz());
		session.fireAllRules();
		logger.close();
    }
    
    public static class Pomiar {
    	public int temperatura;
    	public boolean dym;
    	public Pomiar(int temperatura, boolean dym) {
			this.temperatura = temperatura;
			this.dym = dym;
		}
    }
    
    public static class Pozar {
    }
    
    public static class Telefon {
    	public int numer;
    	public Telefon(int numer) { this.numer = numer; }
    	public int getNumer() { return numer; }
    }
   
    public static class Straz {
    }
}
