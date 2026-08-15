interface Item388 { id: number; name: string; }
type Key388 = string | number;
export function make388(id: number, name: string): Item388 {
  const value: Item388 = {id, name};
  return value as Item388;
}
export const item388: Item388 = make388(388, "item-388");
