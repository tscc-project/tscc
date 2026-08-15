interface Item423 { id: number; name: string; }
type Key423 = string | number;
export function make423(id: number, name: string): Item423 {
  const value: Item423 = {id, name};
  return value as Item423;
}
export const item423: Item423 = make423(423, "item-423");
