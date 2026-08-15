interface Item168 { id: number; name: string; }
type Key168 = string | number;
export function make168(id: number, name: string): Item168 {
  const value: Item168 = {id, name};
  return value as Item168;
}
export const item168: Item168 = make168(168, "item-168");
