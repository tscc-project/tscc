interface Item252 { id: number; name: string; }
type Key252 = string | number;
export function make252(id: number, name: string): Item252 {
  const value: Item252 = {id, name};
  return value as Item252;
}
export const item252: Item252 = make252(252, "item-252");
