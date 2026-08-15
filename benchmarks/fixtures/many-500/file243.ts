interface Item243 { id: number; name: string; }
type Key243 = string | number;
export function make243(id: number, name: string): Item243 {
  const value: Item243 = {id, name};
  return value as Item243;
}
export const item243: Item243 = make243(243, "item-243");
