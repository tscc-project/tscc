interface Item89 { id: number; name: string; }
type Key89 = string | number;
export function make89(id: number, name: string): Item89 {
  const value: Item89 = {id, name};
  return value as Item89;
}
export const item89: Item89 = make89(89, "item-89");
