interface Item307 { id: number; name: string; }
type Key307 = string | number;
export function make307(id: number, name: string): Item307 {
  const value: Item307 = {id, name};
  return value as Item307;
}
export const item307: Item307 = make307(307, "item-307");
