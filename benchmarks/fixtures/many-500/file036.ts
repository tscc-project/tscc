interface Item36 { id: number; name: string; }
type Key36 = string | number;
export function make36(id: number, name: string): Item36 {
  const value: Item36 = {id, name};
  return value as Item36;
}
export const item36: Item36 = make36(36, "item-36");
