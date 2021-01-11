from django import template
#### НЕ ИСПОЛЬЗОВАЛ
register = template.Library()

@register.filter(name='prisv')
def prisv(value, arg):
     arg = str(arg)
     return value.object.filter(pk = person.id)
