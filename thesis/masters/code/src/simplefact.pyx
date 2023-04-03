#
# factpp - Python interface to FaCT++ reasoner
#
# Copyright (C) 2016-2018 by Artur Wroblewski <wrobell@riseup.net>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# distutils: language = c++
# cython: c_string_type=unicode, c_string_encoding=utf8, embedsignature=True, language_level=3

# NOTE: watch out for exceptions: EFaCTPlusPlus, EFPPAssertion, EFPPSaveLoad, EFPPInconsistentKB, EFPPCycleInRIA, EFPPNonSimpleRole, EFPPCantRegName
# EFPPTimeout: checkSatisfiability, runSat, buildCache, isSatisfiable, isSubHolds, buildCompletionTree, checkDisjointRoles, checkIrreflexivity, consistentNominalCloud


from libc.stdlib cimport malloc, free
from libcpp cimport bool
from libcpp.string cimport string
from libcpp.set cimport set
from src.ontofun import *

cdef extern from "<vector>" namespace "std":
    cdef cppclass vector[T]:
        cppclass iterator:
            T operator*()
            iterator operator++()
            bint operator==(iterator)
            bint operator!=(iterator)

        T& operator[](int)
        int size()
        iterator begin()
        iterator end()

cdef extern from 'tNamedEntry.h':
    cdef cppclass TNamedEntry:
        TNamedEntry(string) except +
        string getName()

cdef extern from 'taxNamEntry.h':
    cdef cppclass ClassifiableEntry(TNamedEntry):
        ClassifiableEntry() except +
        bool isSystem()
        bool isTop()
        bool isBottom()
        TNamedEntity *getEntity()

ctypedef ClassifiableEntry EntryType
ctypedef vector[const EntryType*] Array1D
ctypedef vector[Array1D] Array2D

cdef extern from 'Actor.h':
    cdef cppclass Actor:
        Actor() except +
        void getFoundData(Array1D& array) const
        void getFoundData(Array2D& array) const

cdef extern from 'tDLAxiom.h':
    cdef cppclass TDLAxiom:
        TDLAxiom() except +

cdef extern from 'tConcept.h':
    cdef cppclass TConcept(ClassifiableEntry):
        TConcept(string) except +
        bool isSingleton()

cdef extern from 'tIndividual.h':
    cdef cppclass TRelatedMap:
        TRelatedMap() except +

    cdef cppclass TIndividual(TConcept):
        TIndividual(string) except +

cdef extern from 'tRole.h':
    cdef cppclass TRole:
        TRole(string) except +

cdef extern from 'tIndividual.h' namespace 'TRelatedMap':
    ctypedef vector[TIndividual*] CIVec

cdef extern from 'tDLExpression.h':
    cdef cppclass TNamedEntity:
        string getName()

    cdef cppclass TDLObjectRoleName:
        TDLObjectRoleName(string) except +
        string getName()

    cdef cppclass TDLExpression:
        pass

    cdef cppclass TDLRoleExpression(TDLExpression):
        pass

    cdef cppclass TDLObjectRoleComplexExpression(TDLRoleExpression):
        pass

    cdef cppclass TDLObjectRoleExpression(TDLObjectRoleComplexExpression):
        pass

    cdef cppclass TDLObjectRoleName(TDLObjectRoleExpression):
        pass

    cdef cppclass TDLDataExpression:
        pass

    cdef cppclass TDLDataValue(TDLDataExpression):
        pass

    cdef cppclass TDLDataTypeExpression(TDLDataExpression):
        pass

    cdef cppclass TDLDataTypeName(TDLDataTypeExpression):
        pass

    cdef cppclass TDLConceptExpression(TDLExpression):
        pass

    cdef cppclass TDLConceptName(TDLConceptExpression):
        string getName()

    cdef cppclass TDLIndividualExpression(TDLExpression):
        pass

    cdef cppclass TDLIndividualName(TDLIndividualExpression):
        TDLIndividualName(string) except +
        string getName()

    cdef cppclass TDLDataRoleExpression(TDLExpression):
        pass

    cdef cppclass TDLDataRoleName(TDLDataRoleExpression):
        string getName()

cdef extern from 'tExpressionManager.h':
    cdef cppclass TExpressionManager:
        TDLConceptName* Concept(string)
        TDLIndividualExpression* Individual(string)
        TDLDataRoleName* DataRole(string)
        TDLObjectRoleName* ObjectRole(string)
        TDLDataExpression* DataTop()
        TDLDataExpression* DataBottom()
        TDLDataTypeName* DataType(string)

        TDLConceptExpression* Top()
        TDLConceptExpression* Bottom()
        TDLConceptExpression* Forall(const TDLObjectRoleExpression*, const TDLConceptExpression*)
        TDLConceptExpression* Exists(const TDLObjectRoleExpression*, const TDLConceptExpression*)
        TDLConceptExpression* Exists(const TDLDataRoleExpression*, const TDLDataExpression*)
        TDLObjectRoleExpression* Inverse(const TDLObjectRoleExpression*)
        TDLObjectRoleComplexExpression* Compose()

        TDLConceptExpression* And()
        TDLConceptExpression* And(const TDLConceptExpression*, const TDLConceptExpression*)
        TDLConceptExpression* Or()
        TDLConceptExpression* Or(const TDLConceptExpression*, const TDLConceptExpression*)
        TDLConceptExpression* Not(const TDLConceptExpression*)
        TDLConceptExpression* OneOf()

        TDLConceptExpression* Cardinality(unsigned int, const TDLDataRoleExpression*, const TDLDataExpression*)
        TDLConceptExpression* MinCardinality(unsigned int, const TDLObjectRoleExpression*, const TDLConceptExpression*)
        TDLConceptExpression* MinCardinality(unsigned int, const TDLDataRoleExpression*, const TDLDataExpression*)
        TDLConceptExpression* MaxCardinality(unsigned int, const TDLObjectRoleExpression*, const TDLConceptExpression*)
        TDLConceptExpression* MaxCardinality(unsigned int, const TDLDataRoleExpression*, const TDLDataExpression*)

        const TDLDataValue* DataValue(string, const TDLDataTypeExpression*);

        void newArgList()
        void addArg(const TDLExpression*)

cdef extern from 'taxVertex.h':
    cdef cppclass TaxonomyVertex:
        TaxonomyVertex() except +
        const ClassifiableEntry* getPrimer()
        vector[TaxonomyVertex*].iterator begin(bool)
        vector[TaxonomyVertex*].iterator end(bool)

cdef extern from 'Kernel.h':
    cdef cppclass ReasoningKernel:
        ReasoningKernel() except +

        void setOperationTimeout(unsigned long)
        void setIgnoreExprCache(bool)
        void setUseIncrementalReasoning(bool)
        void setVerboseOutput(bool)
        void setUseUndefinedNames(bool)

        TExpressionManager* getExpressionManager()

        TDLAxiom* equalORoles()
        TDLAxiom* equalDRoles()
        TDLAxiom* setSymmetric(TDLObjectRoleExpression*)
        TDLAxiom* setTransitive(TDLObjectRoleExpression*)
        TDLAxiom* setIrreflexive(TDLObjectRoleExpression*)
        TDLAxiom* setOFunctional(TDLObjectRoleExpression*)
        TDLAxiom* setDFunctional(TDLDataRoleExpression*)
        TDLAxiom* setInverseFunctional(TDLObjectRoleExpression*)
        TDLAxiom* relatedTo(TDLIndividualExpression*, TDLObjectRoleExpression*, TDLIndividualExpression*)
        TDLAxiom* relatedToNot(TDLIndividualExpression*, TDLObjectRoleComplexExpression*, TDLIndividualExpression*)
        TDLAxiom* instanceOf(TDLIndividualExpression*, TDLConceptExpression* C)
        bool isSameIndividuals(const TDLIndividualExpression*, const TDLIndividualExpression*)
        bool isInstance(TDLIndividualExpression*, TDLConceptExpression* C)
        TDLAxiom* valueOf(TDLIndividualExpression*, TDLDataRoleExpression*, TDLDataValue*)
        TDLAxiom* impliesORoles(TDLObjectRoleComplexExpression*, TDLObjectRoleExpression*)
        TDLAxiom* impliesDRoles(TDLDataRoleExpression*, TDLDataRoleExpression*)
        bool isSubRoles(const TDLObjectRoleExpression*, const TDLObjectRoleExpression*)
        bool isSubRoles(const TDLDataRoleExpression*, const TDLDataRoleExpression*)
        TDLAxiom* setODomain(TDLObjectRoleExpression*, TDLConceptExpression*)
        TDLAxiom* setORange(TDLObjectRoleExpression*, TDLConceptExpression*)
        TDLAxiom* setDDomain(TDLDataRoleExpression*, TDLConceptExpression*)
        TDLAxiom* setDRange(TDLDataRoleExpression*, TDLDataExpression*)
        TDLAxiom* setInverseRoles(TDLObjectRoleExpression*, TDLObjectRoleExpression*)

        TDLAxiom* equalConcepts()
        TDLAxiom* disjointConcepts()
        TDLAxiom* disjointUnion(TDLConceptExpression*)
        TDLAxiom* processSame()
        TDLAxiom* processDifferent()
        TDLAxiom* impliesConcepts(TDLConceptExpression*, TDLConceptExpression*) except +
        bool isSubsumedBy(TDLConceptExpression*, TDLConceptExpression*) except +
        bool isEquivalent(TDLConceptExpression*, TDLConceptExpression*) except +
        bool isSatisfiable(TDLConceptExpression*) except +
        #TIndividual* getIndividual(TDLIndividualExpression*, char*)
        #TRole* getRole(TDLObjectRoleExpression*, char*)
        #CIVec& getRelated(TIndividual*, TRole*)
        #void getRoleFillers(TDLIndividualExpression*, TDLObjectRoleExpression*, IndividualSet&)
        CIVec& getRoleFillers(TDLIndividualExpression*, TDLObjectRoleExpression*)
        void getTriples(const string&, const string&, const string&, set[vector[string]]&)
        TaxonomyVertex* setUpCache(TDLConceptExpression*)

        void realiseKB()
        void classifyKB()
        bool isKBConsistent() except +
        bool isKBPreprocessed()

        void getEquivalentConcepts(const TDLConceptExpression*, Actor& actor) except +

        void retract(TDLAxiom* axiom)

cdef class Reasoner:
    cdef ReasoningKernel *c_kernel
    cdef TExpressionManager *c_mgr
    cdef TDLAxiom *last_axiom
    cdef TDLConceptName **concepts
    cdef TDLObjectRoleName **roles
    cdef int n_concepts
    cdef int n_roles

    def __cinit__(self):
        self.c_kernel = new ReasoningKernel()
        self.c_mgr = self.c_kernel.getExpressionManager()

    def __dealloc__(self):
        del self.c_kernel
        free(self.concepts)
        free(self.roles)

    def __init__(self, *, n_concepts, n_roles, incremental=None, timeout=None,
                ignore_expr_cache=None, verbose=None, allow_undefined_names=None):
        # preallocate atomic concepts and roles
        self.n_concepts = n_concepts
        self.n_roles = n_roles
        self.concepts = <TDLConceptName**> malloc(n_concepts * sizeof(TDLConceptName*))
        self.roles = <TDLObjectRoleName**> malloc(n_roles * sizeof(TDLObjectRoleName*))
        cdef unsigned int i
        for i in range(n_concepts):
            self.concepts[i] = self.c_mgr.Concept(('C_' + str(i)).encode())
        for i in range(n_roles):
            self.roles[i] = self.c_mgr.ObjectRole(('R_' + str(i)).encode())

        # set options
        if incremental is not None:
            self.c_kernel.setUseIncrementalReasoning(incremental)
        if ignore_expr_cache is not None:
            self.c_kernel.setIgnoreExprCache(ignore_expr_cache)
        if timeout is not None:
            self.c_kernel.setOperationTimeout(int(timeout))
        if verbose is not None:
            self.c_kernel.setVerboseOutput(verbose)
        if allow_undefined_names is not None:
            self.c_kernel.setUseUndefinedNames(allow_undefined_names)

    def set_timeout(self, unsigned long timeout):
        self.c_kernel.setOperationTimeout(timeout)

    def validate(self, expr):
        if expr == TOP or expr == BOT:
            return
        elif isinstance(expr, int):
            assert 0 <= expr < self.n_concepts, f'Unknown concept {expr}'
        elif isinstance(expr, tuple):
            assert len(expr) > 0
            head = expr[0]
            if head == NOT:
                assert len(expr) == 2
                self.validate(expr[1])
            elif head == AND or head == OR:
                assert len(expr) >= 3
                for arg in expr[1:]:
                    self.validate(arg)
            elif head == ANY or head == ALL:
                assert len(expr) == 3
                assert isinstance(expr[1], int) and 0 <= expr[1] < self.n_roles, f'Unknown role {expr[1]}'
                self.validate(expr[2])
            else:
                assert False, f'Unknown constructor {head}'
        else:
            assert False, f'Invalid class expression {expr}'

    cdef TDLConceptExpression *make_expr_unsafe(self, expr):
        if expr == TOP:
            return self.c_mgr.Top()
        elif expr == BOT:
            return self.c_mgr.Bottom()
        elif isinstance(expr, tuple):
            head = expr[0]
            if head == NOT:
                return self.c_mgr.Not(self.make_expr_unsafe(expr[1]))
            if head == AND:
                if len(expr) == 3:
                    return self.c_mgr.And(self.make_expr_unsafe(expr[1]), self.make_expr_unsafe(expr[2]))
                else:
                    self.c_mgr.newArgList()
                    for arg in expr[1:]:
                        self.c_mgr.addArg(self.make_expr_unsafe(arg))
                    return self.c_mgr.And()
            if head ==  OR:
                if len(expr) == 3:
                    return self.c_mgr.Or(self.make_expr_unsafe(expr[1]), self.make_expr_unsafe(expr[2]))
                else:
                    self.c_mgr.newArgList()
                    for arg in expr[1:]:
                        self.c_mgr.addArg(self.make_expr_unsafe(arg))
                    return self.c_mgr.Or()
            if head == ANY:
                return self.c_mgr.Exists(self.roles[expr[1]], self.make_expr_unsafe(expr[2]))
            if head == ALL:
                return self.c_mgr.Forall(self.roles[expr[1]], self.make_expr_unsafe(expr[2]))
        elif isinstance(expr, int):
            return self.concepts[expr]

    cdef TDLConceptExpression *make_expr(self, expr) except *:
        self.validate(expr)
        return self.make_expr_unsafe(expr)  

    def has_unsat_concepts(self):
        cdef TDLConceptExpression *bottom = self.c_mgr.Bottom()
        cdef Actor actor
        self.c_kernel.getEquivalentConcepts(bottom, actor)
        cdef vector[const ClassifiableEntry*] entries
        actor.getFoundData(entries)
        for i in range(entries.size()):
            if not entries[i].isBottom():
                return True
        return False

    def realise(self):
        self.c_kernel.realiseKB()

    def classify(self):
        self.c_kernel.classifyKB()

    def is_consistent(self):
        return self.c_kernel.isKBConsistent()

    def is_coherent(self):
        return self.is_consistent() and not self.has_unsat_concepts()

    def is_preprocessed(self):
        return self.c_kernel.isKBPreprocessed()

    def check_sat(self, c):
        return self.c_kernel.isSatisfiable(self.make_expr(c))

    def check_sub(self, c, d):
        return self.c_kernel.isSubsumedBy(self.make_expr(c), self.make_expr(d))

    def check_eqv(self, c, d):
        return self.c_kernel.isEquivalent(self.make_expr(c), self.make_expr(d))

    def check_axiom(self, axiom):
        op = axiom[0]
        if op == SUB:
            return self.check_sub(axiom[1], axiom[2])
        if op == EQV:
            return self.check_eqv(axiom[1], axiom[2])
        if op == DIS:
            return self.check_sub((AND, axiom[1], axiom[2]), BOT)
        assert False, 'unknown axiom ' + str(op)

    def add_axiom(self, axiom):
        op = axiom[0]
        if op == SUB:
            self.last_axiom = self.c_kernel.impliesConcepts(self.make_expr(axiom[1]), self.make_expr(axiom[2]))
        elif op == EQV:
            self.c_mgr.newArgList()
            for arg in axiom[1:]:
                self.c_mgr.addArg(self.make_expr(arg))
            self.last_axiom = self.c_kernel.equalConcepts()
        elif op == DIS:
            self.c_mgr.newArgList()
            for arg in axiom[1:]:
                self.c_mgr.addArg(self.make_expr(arg))
            self.last_axiom = self.c_kernel.disjointConcepts()
        else:
            assert False, 'unknown axiom ' + str(op)

    def retract_last(self):
        if self.last_axiom != NULL:
            self.c_kernel.retract(self.last_axiom)
            self.last_axiom = NULL

