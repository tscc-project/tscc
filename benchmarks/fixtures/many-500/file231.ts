interface Item231 { id: number; name: string; }
type Key231 = string | number;
export function make231(id: number, name: string): Item231 {
  const value: Item231 = {id, name};
  return value as Item231;
}
export const item231: Item231 = make231(231, "item-231");
