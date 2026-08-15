interface Item103 { id: number; name: string; }
type Key103 = string | number;
export function make103(id: number, name: string): Item103 {
  const value: Item103 = {id, name};
  return value as Item103;
}
export const item103: Item103 = make103(103, "item-103");
