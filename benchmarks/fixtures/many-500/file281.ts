interface Item281 { id: number; name: string; }
type Key281 = string | number;
export function make281(id: number, name: string): Item281 {
  const value: Item281 = {id, name};
  return value as Item281;
}
export const item281: Item281 = make281(281, "item-281");
