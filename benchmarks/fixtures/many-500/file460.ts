interface Item460 { id: number; name: string; }
type Key460 = string | number;
export function make460(id: number, name: string): Item460 {
  const value: Item460 = {id, name};
  return value as Item460;
}
export const item460: Item460 = make460(460, "item-460");
