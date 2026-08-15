interface Item323 { id: number; name: string; }
type Key323 = string | number;
export function make323(id: number, name: string): Item323 {
  const value: Item323 = {id, name};
  return value as Item323;
}
export const item323: Item323 = make323(323, "item-323");
