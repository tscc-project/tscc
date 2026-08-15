interface Item322 { id: number; name: string; }
type Key322 = string | number;
export function make322(id: number, name: string): Item322 {
  const value: Item322 = {id, name};
  return value as Item322;
}
export const item322: Item322 = make322(322, "item-322");
