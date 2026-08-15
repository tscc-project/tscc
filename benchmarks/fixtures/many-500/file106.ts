interface Item106 { id: number; name: string; }
type Key106 = string | number;
export function make106(id: number, name: string): Item106 {
  const value: Item106 = {id, name};
  return value as Item106;
}
export const item106: Item106 = make106(106, "item-106");
