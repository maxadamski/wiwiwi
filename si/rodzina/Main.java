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

		System.out.println("-- back in java --");
		QueryResults results = session.getQueryResults("query stryj");
		for (QueryResultsRow row : results) {
			Relacja rel = (Relacja) row.get("rel");
			System.out.println(rel.o1 + " jest stryjem " + rel.o2);
		}

		logger.close();
    }
    
    public static enum Plec { NIEZNANA, MEZCZYZNA, KOBIETA };
    
    public static class Osoba {
    	public String imie;
    	public Plec plec;
    	public ArrayList<Relacja> relacje = new ArrayList<>();
    	public Osoba(String imie) {
    		this.imie = imie;
    		this.plec = Plec.NIEZNANA;
    	}
    	public String toString() {
    		return imie;
    	}
    }
    
    public static class Relacja {
    	public Osoba o1, o2;
    	public String relacja;
    	public Relacja(Osoba o1, Osoba o2, String relacja) {
    		this.o1 = o1;
    		this.o2 = o2;
    		this.relacja = relacja;
    		this.o1.relacje.add(this);
    		this.o2.relacje.add(this);
    	}
    	public Osoba getO1() {return o1;}
    	public Osoba getO2() {return o1;}
    }
}
