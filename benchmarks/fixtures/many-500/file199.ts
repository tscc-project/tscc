interface Item199 { id: number; name: string; }
type Key199 = string | number;
export function make199(id: number, name: string): Item199 {
  const value: Item199 = {id, name};
  return value as Item199;
}
export const item199: Item199 = make199(199, "item-199");
