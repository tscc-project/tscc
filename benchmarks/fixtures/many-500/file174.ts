interface Item174 { id: number; name: string; }
type Key174 = string | number;
export function make174(id: number, name: string): Item174 {
  const value: Item174 = {id, name};
  return value as Item174;
}
export const item174: Item174 = make174(174, "item-174");
