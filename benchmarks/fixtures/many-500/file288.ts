interface Item288 { id: number; name: string; }
type Key288 = string | number;
export function make288(id: number, name: string): Item288 {
  const value: Item288 = {id, name};
  return value as Item288;
}
export const item288: Item288 = make288(288, "item-288");
