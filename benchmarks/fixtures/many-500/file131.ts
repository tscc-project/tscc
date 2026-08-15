interface Item131 { id: number; name: string; }
type Key131 = string | number;
export function make131(id: number, name: string): Item131 {
  const value: Item131 = {id, name};
  return value as Item131;
}
export const item131: Item131 = make131(131, "item-131");
