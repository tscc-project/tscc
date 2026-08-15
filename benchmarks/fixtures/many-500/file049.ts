interface Item49 { id: number; name: string; }
type Key49 = string | number;
export function make49(id: number, name: string): Item49 {
  const value: Item49 = {id, name};
  return value as Item49;
}
export const item49: Item49 = make49(49, "item-49");
