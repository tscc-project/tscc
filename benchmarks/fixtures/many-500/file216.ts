interface Item216 { id: number; name: string; }
type Key216 = string | number;
export function make216(id: number, name: string): Item216 {
  const value: Item216 = {id, name};
  return value as Item216;
}
export const item216: Item216 = make216(216, "item-216");
