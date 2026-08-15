interface Item384 { id: number; name: string; }
type Key384 = string | number;
export function make384(id: number, name: string): Item384 {
  const value: Item384 = {id, name};
  return value as Item384;
}
export const item384: Item384 = make384(384, "item-384");
