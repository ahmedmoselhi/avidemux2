// Generated by admPyClass.pl from binding/pyDFLabel.admPyClass, DO NOT edit !
tp_obj zzpy__pyDFLabel_get(tp_vm *vm)
{
  tp_obj self = tp_getraw(vm);
  IScriptEngine *engine = (IScriptEngine*)tp_get(vm, vm->builtins, tp_string("userdata")).data.val;
  IEditor *editor = engine->editor();
  TinyParams pm(vm);
  ADM_scriptDFReadOnlyTextHelper *me = (ADM_scriptDFReadOnlyTextHelper *)pm.asThis(&self, ADM_PYID_DF_LABEL);
  char const *key = pm.asString();
  return tp_get(vm, self, tp_string(key));
}
tp_obj zzpy__pyDFLabel_set(tp_vm *vm)
{
  tp_obj self = tp_getraw(vm);
  IScriptEngine *engine = (IScriptEngine*)tp_get(vm, vm->builtins, tp_string("userdata")).data.val;
  IEditor *editor = engine->editor();
  TinyParams pm(vm);
  ADM_scriptDFReadOnlyTextHelper *me = (ADM_scriptDFReadOnlyTextHelper *)pm.asThis(&self, ADM_PYID_DF_LABEL);
  char const *key = pm.asString();
  return tp_None;
}
// Dctor
static void myDtorpyDFLabel(tp_vm *vm,tp_obj self)
{
  ADM_scriptDFReadOnlyTextHelper *cookie = (ADM_scriptDFReadOnlyTextHelper *)self.data.val;
  if (cookie) delete cookie;
  self.data.val = NULL;
}
// Ctor (str)
static tp_obj myCtorpyDFLabel(tp_vm *vm)
{
  tp_obj self = tp_getraw(vm);
  TinyParams pm(vm);
  const char *p0 = pm.asString();
  ADM_scriptDFReadOnlyTextHelper *me = new ADM_scriptDFReadOnlyTextHelper(p0);
  tp_obj cdata = tp_data(vm, ADM_PYID_DF_LABEL, me);
  cdata.data.info->xfree = myDtorpyDFLabel;
  tp_set(vm, self, tp_string("cdata"), cdata);
  return tp_None;
}
static tp_obj zzpy__pyDFLabel_help(TP)
{
  PythonEngine *engine = (PythonEngine*)tp_get(tp, tp->builtins, tp_string("userdata")).data.val;

  engine->callEventHandlers(IScriptEngine::Information, NULL, -1, "constructor:\n");
  engine->callEventHandlers(IScriptEngine::Information, NULL, -1, "obj	DFLabel(str title)\n");

  return tp_None;
}
tp_obj initClasspyDFLabel(tp_vm *vm)
{
  tp_obj myClass = tp_class(vm);
  tp_set(vm,myClass, tp_string("__init__"), tp_fnc(vm,myCtorpyDFLabel));
  tp_set(vm,myClass, tp_string("__set__"), tp_fnc(vm,zzpy__pyDFLabel_set));
  tp_set(vm,myClass, tp_string("__get__"), tp_fnc(vm,zzpy__pyDFLabel_get));
  tp_set(vm,myClass, tp_string("help"), tp_fnc(vm,zzpy__pyDFLabel_help));
  return myClass;
}
